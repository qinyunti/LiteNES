#ifndef LCD_RTP_H
#define LCD_RTP_H

#include <stdint.h>

int lcd_init(char* ip,int port);
void lcd_clear(uint32_t color);
void lcd_setpixel(int index,int x, int y,uint32_t color);
void lcd_switch(int index);
#endif