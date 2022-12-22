#ifndef LCD_FRAMEBUFFER_H
#define LCD_FRAMEBUFFER_H

#include <stdint.h>

int lcd_init(char* dev);
void lcd_clear(uint32_t color);
void lcd_setpixel(int index,int x, int y,uint32_t color);
void lcd_switch(int index);
#endif