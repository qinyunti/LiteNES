#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "dis.h"


#define DIM 1024
#define DM1 (DIM-1)
#define _sq(x) ((x)*(x)) // square
#define _cb(x) abs((x)*(x)*(x)) // absolute value of cube
#define _cr(x) (unsigned char)(pow((x),1.0/3.0)) // cube root


unsigned char RD1(int i,int j)
{
  return (char)(_sq(cos(atan2(j-512,i-512)/2))*255);
}

unsigned char GR1(int i,int j)
{
  return (char)(_sq(cos(atan2(j-512,i-512)/2-2*acos(-1)/3))*255);
}

unsigned char BL1(int i,int j)
{
  return (char)(_sq(cos(atan2(j-512,i-512)/2+2*acos(-1)/3))*255);
}

int draw1(void)
{
    for(int j=0;j<DIM;j++)
    {
        for(int i=0;i<DIM;i++)
        {
            dis_set_pixel(j,i,RD1(i,j)&255,GR1(i,j)&255,BL1(i,j)&255);
        }
    }
}


unsigned char RD2(int i,int j)
{
  #define r(n)(rand()%n)
  static char c[1024][1024];
  return!c[i][j]?c[i][j]=!r(999)?r(256):RD2((i+r(2))%1024,(j+r(2))%1024):c[i][j];
}

unsigned char GR2(int i,int j)
{
  static char c[1024][1024];
  return!c[i][j]?c[i][j]=!r(999)?r(256):GR2((i+r(2))%1024,(j+r(2))%1024):c[i][j];
}

unsigned char BL2(int i,int j)
{
  static char c[1024][1024];
  return!c[i][j]?c[i][j]=!r(999)?r(256):BL2((i+r(2))%1024,(j+r(2))%1024):c[i][j];
}

int draw2(void)
{
    for(int j=0;j<DIM;j++)
    {
        for(int i=0;i<DIM;i++)
        {
            dis_set_pixel(j,i,RD2(i,j)&255,GR2(i,j)&255,BL2(i,j)&255);
        }
    }
}

unsigned char RD3(int i,int j)
{
  float x=0,y=0;int k;for(k=0;k++<256;){float a=x*x-y*y+(i-768.0)/512;y=2*x*y+(j-512.0)/512;x=a;if(x*x+y*y>4)break;}
  return log(k)*47;
}

unsigned char GR3(int i,int j)
{
  float x=0,y=0;int k;for(k=0;k++<256;){float a=x*x-y*y+(i-768.0)/512;y=2*x*y+(j-512.0)/512;x=a;if(x*x+y*y>4)break;}
  return log(k)*47;
}

unsigned char BL3(int i,int j)
{
  float x=0,y=0;int k;for(k=0;k++<256;){float a=x*x-y*y+(i-768.0)/512;y=2*x*y+(j-512.0)/512;x=a;if(x*x+y*y>4)break;}
  return 128-log(k)*23;
}

int draw3(void)
{
    for(int j=0;j<DIM;j++)
    {
        for(int i=0;i<DIM;i++)
        {
            dis_set_pixel(j,i,RD3(i,j)&255,GR3(i,j)&255,BL3(i,j)&255);
        }
    }
}

unsigned char RD4(int i,int j)
{
  double a=0,b=0,c,d,n=0;
  while((c=a*a)+(d=b*b)<4&&n++<880)
  {b=2*a*b+j*8e-9-.645411;a=c-d+i*8e-9+.356888;}
  return 255*pow((n-80)/800,3.);
}

unsigned char GR4(int i,int j)
{
  double a=0,b=0,c,d,n=0;
  while((c=a*a)+(d=b*b)<4&&n++<880)
  {b=2*a*b+j*8e-9-.645411;a=c-d+i*8e-9+.356888;}
  return 255*pow((n-80)/800,.7);
}

unsigned char BL4(int i,int j)
{
  double a=0,b=0,c,d,n=0;
  while((c=a*a)+(d=b*b)<4&&n++<880)
  {b=2*a*b+j*8e-9-.645411;a=c-d+i*8e-9+.356888;}
  return 255*pow((n-80)/800,.5);
}

int draw4(void)
{
    for(int j=0;j<DIM;j++)
    {
        for(int i=0;i<DIM;i++)
        {
            dis_set_pixel(j,i,RD4(i,j)&255,GR4(i,j)&255,BL4(i,j)&255);
        }
    }
}


unsigned char RD5(int i,int j)
{
  static double k;k+=rand()/1./RAND_MAX;int l=k;l%=512;return l>255?511-l:l;
}

unsigned char GR5(int i,int j)
{
  static double k;k+=rand()/1./RAND_MAX;int l=k;l%=512;return l>255?511-l:l;
}

unsigned char BL5(int i,int j)
{
  static double k;k+=rand()/1./RAND_MAX;int l=k;l%=512;return l>255?511-l:l;
}

int draw5(void)
{
    for(int j=0;j<DIM;j++)
    {
        for(int i=0;i<DIM;i++)
        {
            dis_set_pixel(j,i,RD5(i,j)&255,GR5(i,j)&255,BL5(i,j)&255);
        }
    }
}

unsigned char RD6(int i,int j)
{
  float s=3./(j+99);
  float y=(j+sin((i*i+_sq(j-700)*5)/100./DIM)*35)*s;
  return ((int)((i+DIM)*s+y)%2+(int)((DIM*2-i)*s+y)%2)*127;
}

unsigned char GR6(int i,int j)
{
  float s=3./(j+99);
  float y=(j+sin((i*i+_sq(j-700)*5)/100./DIM)*35)*s;
  return ((int)(5*((i+DIM)*s+y))%2+(int)(5*((DIM*2-i)*s+y))%2)*127;
}

unsigned char BL6(int i,int j)
{
  float s=3./(j+99);
  float y=(j+sin((i*i+_sq(j-700)*5)/100./DIM)*35)*s;
  return ((int)(29*((i+DIM)*s+y))%2+(int)(29*((DIM*2-i)*s+y))%2)*127;
}

int draw6(void)
{
    for(int j=0;j<DIM;j++)
    {
        for(int i=0;i<DIM;i++)
        {
            dis_set_pixel(j,i,RD6(i,j)&255,GR6(i,j)&255,BL6(i,j)&255);
        }
    }
}

unsigned char BL7(int i,int j);
unsigned char RD7(int i,int j)
{
    return(i+j)?256-(BL7(i,j))/2:0;
}

unsigned char GR7(int i,int j)
{
    return RD7(i,j);
}

unsigned char BL7(int i,int j)
{
    static int m[DIM][DIM],e,x,y,d,c[4],f,n;
    if(i+j<1)
    {
        for(d=DIM*DIM;d;d--)
        {
            m[d%DIM][d/DIM]=d%6?0:rand()%2000?1:255;
        }
        for(n=1;n;n++)
        {
            x=rand()%DIM;
            y=rand()%DIM;
            if(m[x][y]==1)
            {
                f=1;
                for(d=0;d<4;d++)
                {
                    c[d]=m[(x+DIM+(d==0)-(d==2))%DIM][(y+DIM+(d==1)-(d==3))%DIM];
                    f=f<c[d]?c[d]:f;
                }
                if(f>2)
                {
                    m[x][y]=f-1;
                }
                else
                {
                    ++e;
                    e%=4;
                    d=e;
                    if(!c[e])
                    {
                        m[x][y]=0;
                        m[(x+DIM+(d==0)-(d==2))%DIM][(y+DIM+(d==1)-(d==3))%DIM]=1;
                    }
                }
            }
        }
    }
    return m[i][j];
}

int draw7(void)
{
    for(int j=0;j<DIM;j++)
    {
        for(int i=0;i<DIM;i++)
        {
            dis_set_pixel(j,i,RD7(i,j)&255,GR7(i,j)&255,BL7(i,j)&255);
        }
    }
}

unsigned char BL8(int i,int j);
unsigned char RD8(int i,int j)
{
    return BL8(i,j)*(DIM-i)/DIM;
}

unsigned char GR8(int i,int j)
{
    return BL8(i,j)*(DIM-j/2)/DIM;
}

unsigned char BL8(int i,int j)
{
    static float c[DIM][DIM];
    if(i+j<1)
    {
        float a=0,b,k,r,x;
        int e,o;
        for(;a<DIM;a+=0.1)
        {
            for(b=0;b<DIM;b++)
            {
                r=a*1.6/DIM+2.4;x=1.0001*b/DIM;
                for(k=0;k<DIM;k++)
                {
                    x=r*x*(1-x);
                    if(k>DIM/2)
                    {
                        e=a;
                        o=(DM1*x);
                        c[e][o]+=0.01;
                    }
                }
            }
        }
    }
    return c[j][i]>255?255:c[j][i]*i/DIM;
}


int draw8(void)
{
    for(int j=0;j<DIM;j++)
    {
        for(int i=0;i<DIM;i++)
        {
            dis_set_pixel(j,i,RD8(i,j)&255,GR8(i,j)&255,BL8(i,j)&255);
        }
    }
}

int main(int argc, char* argv[])
{
    uint8_t r= 0x00;
    uint8_t g= 0x00;
    uint8_t b= 0xFF;
    if(argc != 3)
    {
        printf("usage:./lcdtest ip port ie:./lcdtest 192.168.1.101 5004 \n");
    }
    dis_init(argv[1],atoi(argv[2]));
    while(1)
    {
        /* RGB */
        for(int i=0;i<DIS_WIDTH;i++)
        {
            for(int j=0;j<DIS_WIDTH;j++)
            {
                dis_set_pixel(i,j, 0xFF, 0, 0);
            }
        }
        dis_flush();
        sleep(1);

        for(int i=0;i<DIS_WIDTH;i++)
        {
            for(int j=0;j<DIS_WIDTH;j++)
            {
                dis_set_pixel(i,j, 0, 0xFF, 0);
            }
        }
        dis_flush();
        sleep(1);

        for(int i=0;i<DIS_WIDTH;i++)
        {
            for(int j=0;j<DIS_WIDTH;j++)
            {
                dis_set_pixel(i,j, 0, 0, 0xFF);
            }
        }
        dis_flush();
        sleep(1);

        /* PIC */
        draw1();
        dis_flush();
        sleep(1);
        draw2();
        dis_flush();
        sleep(1);
        draw3();
        dis_flush();
        sleep(1);
        draw4();
        dis_flush();
        sleep(1);
        draw5();
        dis_flush();
        sleep(1);
        draw6();
        dis_flush();
        sleep(1);
        draw7();
        dis_flush();
        sleep(1);
        draw8();
        dis_flush();
        sleep(1);
    }
}







      
