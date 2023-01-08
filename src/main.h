#ifndef MAIN_H
#define MAIN_H

#define USE_LCD_RTP 0

#if USE_LCD_RTP
#include "lcd_rtp.h"
#else
#include "lcd_framebuffer.h"
#endif

#endif