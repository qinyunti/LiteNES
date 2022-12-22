#ifndef DIS_H
#define DIS_H

#define DIS_WIDTH 256
#define DIS_HEIGHT 240

void dis_set_pixel(int x,int y, uint8_t r,uint8_t g,uint8_t b);
int dis_init(char* ip,int port);
void dis_flush(void);
#endif