#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "x264.h"
#include "rtp.h"
#include "dis.h"

uint8_t s_rgb_buffer_au8[DIS_WIDTH*DIS_HEIGHT*3];        /**< 显示RGB缓存    */
uint8_t s_yuv_buffer_au8[DIS_WIDTH*DIS_HEIGHT*3/2];      /**< 显示I420缓存   */
uint8_t s_h264_buffer_au8[DIS_WIDTH*DIS_HEIGHT*3*10];    /**< 显示I420缓存   */

uint8_t s_flush_u8 = 0;

static void rgb_to_yuv420p(uint8_t* yuv420p, uint8_t* rgb, int width, int height)
{
	if (yuv420p == NULL || rgb == NULL)
		return;
	int frameSize = width*height;
	int chromaSize = frameSize / 4;
 
	int yIndex = 0;
	int uIndex = frameSize;
	int vIndex = frameSize + chromaSize;
 
	int R, G, B, Y, U, V;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			B = rgb[(i * width + j) * 3 + 0];
			G = rgb[(i * width + j) * 3 + 1];
			R = rgb[(i * width + j) * 3 + 2];
 
			//RGB to YUV
			Y = ((66 * R + 129 * G + 25 * B + 128) >> 8) + 16;
			U = ((-38 * R - 74 * G + 112 * B + 128) >> 8) + 128;
			V = ((112 * R - 94 * G - 18 * B + 128) >> 8) + 128;
 
			yuv420p[yIndex++] = (unsigned char)((Y < 0) ? 0 : ((Y > 255) ? 255 : Y));
			if (i % 2 == 0 && j % 2 == 0)
			{
				yuv420p[uIndex++] = (unsigned char)((U < 0) ? 0 : ((U > 255) ? 255 : U));
				yuv420p[vIndex++] = (unsigned char)((V < 0) ? 0 : ((V > 255) ? 255 : V));
			}
		}
	}
}

static int s_h264_len_i = -1;
static void* dis_llopback(void* arg)
{

	while(1)
	{
#if 0
		if(s_flush_u8 != 0)
		{
			s_flush_u8 = 0;
			rgb_to_yuv420p(s_yuv_buffer_au8, s_rgb_buffer_au8, DIS_WIDTH, DIS_HEIGHT);
			memset(s_h264_buffer_au8,0,sizeof(s_h264_buffer_au8));
			s_h264_len_i = x264_yuv420p2h264(s_yuv_buffer_au8,s_h264_buffer_au8,sizeof(s_h264_buffer_au8),DIS_WIDTH,DIS_HEIGHT);
			printf("flush frame len=%d\n",s_h264_len_i);
		}
		if(s_h264_len_i > 0)
		{
			uint8_t* tmp = malloc(s_h264_len_i+1);
			memcpy(tmp+1,s_h264_buffer_au8,s_h264_len_i);  /**< H264 FU-A分包时会往前写一个字节，改写原始值，所以这里需要临时开辟空间用来发送  */
			rtp_send_h264(tmp+1, s_h264_len_i); 
			free(tmp);
			printf("rtp_send_h264 %d\n",s_h264_len_i);
			/* 帧率控制 */
			usleep(1000000);
		}
#endif
		usleep(1000);
	}
}

void dis_set_pixel(int x,int y, uint8_t r,uint8_t g,uint8_t b)
{
    s_rgb_buffer_au8[(y * DIS_WIDTH + x) * 3 + 0] = b;  /**< r和b反 */
    s_rgb_buffer_au8[(y * DIS_WIDTH + x) * 3 + 1] = g;
    s_rgb_buffer_au8[(y * DIS_WIDTH + x) * 3 + 2] = r;
}

void dis_flush(void)
{
    s_flush_u8 = 1;
	///printf("set flush\n");
	#if 1
		if(s_flush_u8 != 0)
		{
			s_flush_u8 = 0;
			rgb_to_yuv420p(s_yuv_buffer_au8, s_rgb_buffer_au8, DIS_WIDTH, DIS_HEIGHT);
			memset(s_h264_buffer_au8,0,sizeof(s_h264_buffer_au8));
			s_h264_len_i = x264_yuv420p2h264(s_yuv_buffer_au8,s_h264_buffer_au8,sizeof(s_h264_buffer_au8),DIS_WIDTH,DIS_HEIGHT);
			//printf("flush frame len=%d\n",s_h264_len_i);
		}
		if(s_h264_len_i > 0)
		{
			uint8_t* tmp = malloc(s_h264_len_i+1);
			memcpy(tmp+1,s_h264_buffer_au8,s_h264_len_i);  /**< H264 FU-A分包时会往前写一个字节，改写原始值，所以这里需要临时开辟空间用来发送  */
			rtp_send_h264(tmp+1, s_h264_len_i); 
			free(tmp);
			/* 帧率控制 */
			
		}
		#endif
		usleep(100);
}

int dis_init(char* ip,int port)
{
    rtp_h264_init(ip,port);
	x264_init(DIS_WIDTH,DIS_HEIGHT);
	pthread_t id;
	/* 创建函数线程，并且指定函数线程要执行的函数 */
	pthread_create(&id,NULL,dis_llopback,0);
}



