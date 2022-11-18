
/* 单独测试:#define KEY_TEST 1
 * 作为接口:#define KEY_TEST 0
 * 编译 aarch64-linux-gnu-gcc key.c -o key -lpthread
 * 运行 chmod +x key
 *      ./key /dev/input/event8
 */

#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#include<pthread.h>

#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define KEY_TEST 0

int s_keys_fd = -1;
uint32_t s_keys_state = 0;


void key_setstate(int code , int vaule, uint32_t* key)
{
	if(vaule == 0)
	{
		switch(code)
		{
			case 296:
				*key &= ~(1u<<3);
			break;
			case 297:
				*key &= ~(1u<<4);
			break;
			case 288:
				*key &= ~(1u<<5);
			break;
			case 289:
				*key &= ~(1u<<8);
			break;
			case 290:
				*key &= ~(1u<<6);
			break;
			case 291:
				*key &= ~(1u<<7);
			break;
			case 292:
				*key &= ~(1u<<1);
			break;
			case 294:
				*key &= ~(1u<<2);
			break;
			default:
			break;
		}
	}
	else
	{
		switch(code)
		{
			case 296:
				*key |= (1u<<3);
			break;
			case 297:
				*key |= (1u<<4);
			break;
			case 288:
				*key |= (1u<<5);
			break;
			case 289:
				*key |= (1u<<8);
			break;
			case 290:
				*key |= (1u<<6);
			break;
			case 291:
				*key |= (1u<<7);
			break;
			case 292:
				*key |= (1u<<1);
			break;
			case 294:
				*key |= (1u<<2);
			break;
			default:
			break;
		}
	}
}

/* 
 * SELECT 296   SELECT   3
 * START  297   START    4
 * UP     288   右上     5
 * RIGHT  289   右右     8
 * DOWN   290   右下     6
 * LEFT   291   右左     7
 * A      292   左前上    1
 * B      294   左前下    2
 */
int key_getstate(int key)
{
	if(s_keys_state & (1u<<key))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void* key_poll(void* arg)
{
	char ret[2];
	struct input_event t;
	s_keys_fd = open((char*)arg, O_RDONLY);
	if(s_keys_fd <= 0)
	{
		printf("open %s device error!\n",(char*)arg);
		return 0;
	}
	while(1)
	{
		if(read(s_keys_fd, &t, sizeof(t)) == sizeof(t))
		{
			if(t.type==EV_KEY)
            {
				if(t.value==0 || t.value==1)
				{
					key_setstate(t.code, t.value, &s_keys_state);
					///printf("key %d %s\n", t.code, (t.value) ? "Pressed" : "Released");
					//if(t.code == KEY_ESC)
					//	break;
				}
            }
			else
			{
				///printf("type %d code %d value %d\n", t.type, t.code, t.value);
			}
		}
	}
	return 0;
}

void key_init(void* arg)
{
	pthread_t id;
	/* 创建函数线程，并且指定函数线程要执行的函数 */
	int res = pthread_create(&id,NULL,key_poll,arg);
	if(res !=0 )
	{
		printf("pthread_create err\r\n");
	}
}


#if KEY_TEST
int main(int argc, char* argv[])
{
	key_init(argv[1]);
	while(1);
}
#endif