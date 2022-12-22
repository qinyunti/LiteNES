#ifndef RTP_H
#define RTP_H

int rtp_h264_init(char* ip,int port);
int rtp_h264_deinit(void);
int rtp_send_h264(uint8_t* src, uint32_t len);

#endif