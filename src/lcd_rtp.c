
#include <stdint.h>
#include "dis.h"

int lcd_init(char* ip,int port)
{
    dis_init(ip,port);
}

void lcd_clear(uint32_t color)
{
    for(int i=0;i<DIS_WIDTH;i++)
    {
        for(int j=0;j<DIS_WIDTH;j++)
        {
            dis_set_pixel(i,j, (color>>16) & 0xFF,(color>>8) & 0xFF, color & 0xFF);
        }
    }
}

void lcd_setpixel(int index,int x, int y,uint32_t color)
{
    dis_set_pixel(x,y, (color>>16) & 0xFF,(color>>8) & 0xFF, color & 0xFF);
}

void lcd_switch(int index)
{
    if(index==0)
    {

    }
    else
    {

    }
    dis_flush();
}