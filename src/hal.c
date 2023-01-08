/*
This file presents all abstractions needed to port LiteNES.
  (The current working implementation uses allegro library.)

To port this project, replace the following functions by your own:
1) nes_hal_init()
    Do essential initialization work, including starting a FPS HZ timer.

2) nes_set_bg_color(c)
    Set the back ground color to be the NES internal color code c.

3) nes_flush_buf(*buf)
    Flush the entire pixel buf's data to frame buffer.

4) nes_flip_display()
    Fill the screen with previously set background color, and
    display all contents in the frame buffer.

5) wait_for_frame()
    Implement it to make the following code is executed FPS times a second:
        while (1) {
            wait_for_frame();
            do_something();
        }

6) int nes_key_state(int b) 
    Query button b's state (1 to be pressed, otherwise 0).
    The correspondence of b and the buttons:
      0 - Power
      1 - A
      2 - B
      3 - SELECT
      4 - START
      5 - UP
      6 - DOWN
      7 - LEFT
      8 - RIGHT
*/
#include <unistd.h>
#include "hal.h"
#include "fce.h"
#include "common.h"
#include "main.h"
#include "key.h"
#include <sys/time.h>

typedef uint32_t COLOR;
COLOR color_map[64];
static int sx = 1;
static int sy = 1;

long long pre_time = 0;
long long now_time = 0;
long long total_time = 0;
int fnum = 0;

long long get_ms(void)
{
    struct timeval t;
    gettimeofday(&t,NULL);
    return (long long)t.tv_sec * 1000 + (long long)t.tv_usec / 1000;
}

/* Wait until next allegro timer event is fired. 
 * fce_run中循环调用该函数,用该函数延时控制1/FPS(S)执行一次
 */
void wait_for_frame()
{
    now_time = get_ms();
    if((now_time - pre_time) < 1000ul/FPS)
    {
        //usleep((1000ul/FPS - (now_time - pre_time))*1000);
    }
    fnum++;
    total_time += (now_time - pre_time);
    if(fnum >= 1)
    {
        printf("ftime=%dms\r\n",total_time/fnum);
        fnum=0;
        total_time=0;
    }
    pre_time = now_time;
}

/* Set background color. RGB value of c is defined in fce.h 
 * 将整个屏幕填充一种颜色
 */
void nes_set_bg_color(int c)
{
    if(c >= sizeof(color_map)/sizeof(color_map[0]))
    {
        fprintf(stderr,"nes_set_bg_color c:%d\r\n",c);
    }
    lcd_clear(color_map[c]);
}

static int s_lcd_index = 0; /* 当前正在显示的LCD */

/* Flush the pixel buffer 
 * 将缓冲区写入LCD
 */
void nes_flush_buf(PixelBuf *buf) {
    int i;
    for (i = 0; i < buf->size; i ++) {
        Pixel *p = &buf->buf[i];
        int x = p->x, y = p->y;
        if(p->c >= sizeof(color_map)/sizeof(color_map[0]))
        {
            fprintf(stderr,"nes_flush_buf c:%d\r\n",p->c);
        }
        COLOR c = color_map[p->c];
    #if 0
        if(s_lcd_index == 0)
        {
            for(int ix=0;ix<sx;ix++)
            {
                for(int iy=0;iy<sy;iy++)
                {
                    lcd_setpixel(1,x*sx+ix,y*sy+iy,c);
                }
            }
        }
    #endif
        //else
        {
            for(int ix=0;ix<sx;ix++)
            {
                for(int iy=0;iy<sy;iy++)
                {
                    lcd_setpixel(0,x*sx+ix,y*sy+iy,c);
                }
            }
        }
    }
}

/* Initialization:
   (1) start a 1/FPS Hz timer. 
   (2) register fce_timer handle on each timer event */
void nes_hal_init()
{
    for(int i=0;i<64;i++)
    {
        color_map[i] = ((uint32_t)palette[i].r<<16) | ((uint32_t)palette[i].g<<8) | ((uint32_t)palette[i].b<<0);
    }
    uint32_t x;
    uint32_t y;
    lcd_getsize(&x,&y);
    sx = x / 256;
    sy = y / 256;
}

/* Update screen at FPS rate by allegro's drawing function. 
   Timer ensures this function is called FPS times a second. */
void nes_flip_display()
{
    /* 在nes_flush_buf后调用  
     * 真正将数据写入LCD显示 nes_flush_buf中已经显示了这里不再显示
     */
#if 0
    if(s_lcd_index == 0)
    {
        s_lcd_index = 1;
    }
    else
    {
        s_lcd_index = 0;
    }
    lcd_switch(s_lcd_index);
#endif
}

/* Query a button's state.
   Returns 1 if button #b is pressed. */
int nes_key_state(int b)
{
    switch (b)
    {
        case 0: // On / Off
            return 1;
        case 1: // A
        case 2: // B
        case 3: // SELECT
        case 4: // START
        case 5: // UP
        case 6: // DOWN
        case 7: // LEFT
        case 8: // RIGHT
         return key_getstate(b);
        default:
            return 1;
    }
}

