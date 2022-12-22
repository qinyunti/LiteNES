#include <ortp/ortp.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdint.h>

#define MAX_RTP_PKT_LENGTH (1400)
#define TS_INC (90000/25)

static RtpSession *session;
static uint32_t user_ts=0;

static void forward_h264frame(RtpSession * session, uint8_t * buffer, int len,uint32_t userts) 
{
    unsigned char NALU = buffer[0];
    uint32_t valid_len = len;

    if (len <= MAX_RTP_PKT_LENGTH) {
        /*
            RTP Header + NALU Header + NALU Data; （不包括startcode）
            传入的buffer从NALU Header开始
            RTP Header由rtp_session_send_with_ts添加
        */
        rtp_session_send_with_ts(session, buffer,len,userts);
        //printf("send slice len=%d\r\n",len);
    } else {
        /* FU-A *
         * FU indicator  FU header     DATA
         *                ^
         *               NALU Header   NALU Data
         */
        int packetnum = valid_len / (MAX_RTP_PKT_LENGTH-1);  /* FU-A分片要多加一个字节 */
        if (valid_len % (MAX_RTP_PKT_LENGTH-1) != 0) {
            packetnum += 1;
        }
        int i = 0;
        int pos = -1;
        while (i < packetnum) {
            if (i < packetnum - 1) {
                buffer[pos] = (NALU & 0x60) | 28;
                buffer[pos+1] = (NALU & 0x1f);
                if (0 == i) {
                    buffer[pos+1] |= 0x80;
                }
                rtp_session_send_with_ts(session, &buffer[pos],
                MAX_RTP_PKT_LENGTH, userts);
				//printf("send slice offset=%d,len=%d\r\n",pos,MAX_RTP_PKT_LENGTH);
            } else {
                int iSendLen = len - pos - 1;
                buffer[pos] = (NALU & 0x60) | 28;
                buffer[pos+1] = (NALU & 0x1f);
                buffer[pos+1] |= 0x40;
                rtp_session_send_with_ts(session, &buffer[pos],
                        iSendLen, userts);
				//printf("send slice offset=%d,len=%d\r\n",pos,iSendLen);
            }
            pos += MAX_RTP_PKT_LENGTH-2;  /*有效长度MAX_RTP_PKT_LENGTH-1 在往前预留一个字节存储FU indicator */
            ++i;
        }
    }
}

static uint32_t h264isslice(uint8_t* buffer)
{
    if((buffer[0]==0)&&(buffer[1]==0))
    {
        if(buffer[2]==1)
        {
            return 1;
        }
        else if(buffer[2]==0)
        {
            if(buffer[3]==1)
            {
                return 2;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}

static uint32_t h264getslicelen(uint8_t* buffer,uint32_t len,uint8_t* headlen)
{
    uint8_t type;
    uint32_t offset = 0;
    uint32_t isslice = 0;
    isslice = h264isslice(buffer);
    if(isslice==1)
    {
        offset=3;
		*headlen = 3;
    }
    else if(isslice==2)
    {
        offset=4;
		*headlen = 4;
    }
    else
    {
        return 0;
    }
    uint32_t i;
    for(i=offset;i<len;i++)
    {
        if((buffer[i+0]==0)&&(buffer[i+1]==0))
        {
            if(buffer[i+2]==1)
            {
                return i;
            }
            else if(buffer[i+2]==0)
            {
                if(buffer[i+3]==1)
                {
                    return i;
                }
            }
        }
    }
    return i;
}

int rtp_h264_init(char* ip,int port)
{
	int i;
	char *ssrc;
	int clockslide=0;
	int jitter=0;
	ortp_init();
	ortp_scheduler_init();
	ortp_set_log_level_mask(ORTP_ERROR);
	session=rtp_session_new(RTP_SESSION_SENDONLY);	
	rtp_session_set_scheduling_mode(session,1);
	rtp_session_set_blocking_mode(session,1);
	rtp_session_set_connected_mode(session,TRUE);
	rtp_session_set_remote_addr(session,ip,port);
	rtp_session_set_payload_type(session,96);
	ssrc=getenv("SSRC");
	if (ssrc!=NULL) 
    {
		printf("using SSRC=%i.\n",atoi(ssrc));
		rtp_session_set_ssrc(session,atoi(ssrc));
        printf("rtp_session_set_ssrc %s\n",ssrc);
	}
}

int rtp_h264_deinit(void)
{
    rtp_session_destroy(session);
	ortp_exit();
	ortp_global_stats_display();
}

int rtp_send_h264(uint8_t* src, uint32_t len)
{
	uint8_t* src_buff = src;
	uint32_t framelen = 0;
	uint32_t s_sendlen_u32 = 0;
	uint8_t headlen;
	while(1)
	{
        if((framelen = h264getslicelen(src_buff,len-s_sendlen_u32,&headlen)) == 0)
        {
            /* 无有效数据 */
            break;
        }
        forward_h264frame(session,src_buff+headlen,framelen-headlen,user_ts);
		//user_ts+=(framelen-headlen)/TS_INC;
        user_ts+=TS_INC;
        src_buff += framelen;
        s_sendlen_u32+=framelen;
        if(s_sendlen_u32 >= len)
        {
            break;
        }
	}
	return s_sendlen_u32;
}
