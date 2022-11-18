
/* 单独测试:#define LCD_TEST 1
 * 作为接口:#define LCD_TEST 0
 * 编译 aarch64-linux-gnu-gcc lcd.c -o lcd
 * 运行 chmod +x lcd
 *      ./lcd /dev/fb0
 */
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/fb.h>

#define LCD_TEST 0

static int s_fd = -1;
static uint8_t *s_pfb = 0;
static uint32_t s_len_u32 = 0;
struct fb_fix_screeninfo finfo;
struct fb_var_screeninfo vinfo;

void lcd_clear(uint32_t color);
int lcd_init(char* dev)
{
    int ret = -1;
    memset(&finfo,0,sizeof(finfo));
    memset(&vinfo,0,sizeof(vinfo));
    s_fd = open(dev,O_RDWR);
    if(s_fd < 0)
    {
        fprintf(stderr,"open %s err, %s\n",dev,strerror(errno));
        return -1;
    }
    ret = ioctl(s_fd, FBIOGET_FSCREENINFO, &finfo);
    if(ret < 0)
    {
        fprintf(stderr,"ioctl finfo err, %s\n",strerror(errno));
        close(s_fd);
        return -1;
    }
    ret = ioctl(s_fd, FBIOGET_VSCREENINFO, &vinfo);
    if(ret < 0)
    {
        fprintf(stderr,"ioctl vinfo err, %s\n",strerror(errno));
        close(s_fd);
        return -1;
    }
    vinfo.xoffset = 0;
    vinfo.yoffset = 0;
    ret = ioctl(s_fd, FBIOPAN_DISPLAY, &vinfo);
    if(ret < 0)
    {
        fprintf(stderr,"ioctl vinfo err, %s\n",strerror(errno));
        close(s_fd);
        return -1;
    }

    s_len_u32 = vinfo.xres_virtual  * vinfo.yres_virtual * vinfo.bits_per_pixel / 8;
    s_pfb = mmap(NULL, s_len_u32, PROT_READ | PROT_WRITE, MAP_SHARED, s_fd, 0);
    if(s_pfb == NULL)
    {
        fprintf(stderr,"mmap len %u err\n",s_len_u32);
        close(s_fd);
        return -1;
    }
    //fprintf(stderr,"x = %d y = %d vx = %d vy = %d bits = %d len = %ld\n",vinfo.xres,vinfo.yres,vinfo.xres_virtual,vinfo.yres_virtual,vinfo.bits_per_pixel,len);
    fprintf(stderr,"x = %d, y = %d\r\n",vinfo.xres,vinfo.yres);
    fprintf(stderr,"vx = %d, vy = %d\r\n",vinfo.xres_virtual,vinfo.yres_virtual);
    fprintf(stderr,"ox = %d, oy = %d\r\n",vinfo.xoffset,vinfo.yoffset);
    fprintf(stderr,"bits = %d\r\n",vinfo.bits_per_pixel);
    fprintf(stderr,"gray = %d\r\n",vinfo.grayscale);
    fprintf(stderr,"red offset:%d length:%d msb_right:%d\r\n",vinfo.red.offset,vinfo.red.length,vinfo.red.msb_right);
    fprintf(stderr,"green offset:%d length:%d msb_right:%d\r\n",vinfo.green.offset,vinfo.green.length,vinfo.green.msb_right);  
    fprintf(stderr,"blue offset:%d length:%d msb_right:%d\r\n",vinfo.blue.offset,vinfo.blue.length,vinfo.blue.msb_right);  
    fprintf(stderr,"transp offset:%d length:%d msb_right:%d\r\n",vinfo.transp.offset,vinfo.transp.length,vinfo.transp.msb_right); 
    
    lcd_clear(0x000000);
    ///sleep(2);
    ///lcd_clear(0xff371a);
    ///sleep(2);
    return 0;
}

void lcd_deinit(void)
{
    if(s_pfb != 0)
    {
        munmap(s_pfb,s_len_u32);
    }
    if(s_fd > 0)
    {
        close(s_fd);
    }
}


void lcd_setpixel(int index,int x, int y,uint32_t color)
{
    uint32_t offset;
    if(index == 0)
    {
        offset=0;

    }
    else
    {
        offset = vinfo.xres*vinfo.yres*vinfo.bits_per_pixel/8;
    }
    if(x>vinfo.xres || y>vinfo.yres)
    {
        return;
    }
    if(vinfo.bits_per_pixel == 16)
    {
        *((uint16_t*)(s_pfb + offset + (y*vinfo.xres + x)*2)) = (uint16_t)color;
    }
    else
    {
        *((uint32_t*)(s_pfb + offset + (y*vinfo.xres + x)*4)) = 0xFF000000 | color;
    } 
}

void lcd_switch(int index)
{
    if(index==0)
    {
        vinfo.yoffset = 0;
    }
    else
    {
        vinfo.yoffset = vinfo.yres;
    }
    ioctl(s_fd, FBIOPAN_DISPLAY, &vinfo);
}

void lcd_clear(uint32_t color)
{
    for(int i=0;i<vinfo.xres;i++)
    {
        for(int j=0;j<vinfo.yres;j++)
        {
            lcd_setpixel(0,i, j,color);
            lcd_setpixel(1,i, j,color);
        }
    }
}

void lcd_getsize(uint32_t* x,uint32_t* y)
{
    *x = vinfo.xres;
    *y = vinfo.yres;
}


#if LCD_TEST
int main(int argc, char* argv[])
{
	lcd_init(argv[1]);
    lcd_clear(0xFFFF0000);
    sleep(1);
    lcd_clear(0xFF00FF00);
    sleep(1);
    lcd_clear(0xFF0000FF);
    sleep(1);

    for(int i=0;i<vinfo.xres;i++)
    {
        for(int j=0;j<vinfo.yres;j++)
        {
            lcd_setpixel(i, j, (i<<12 | j));
        }
    }
}
#endif