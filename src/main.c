/*
LiteNES originates from Stanislav Yaglo's mynes project:
  https://github.com/yaglo/mynes

LiteNES is a "more portable" version of mynes.
  all system(library)-dependent code resides in "hal.c" and "main.c"
  only depends on libc's memory moving utilities.

How does the emulator work?
  1) read file name at argv[1]
  2) load the rom file into array rom
  3) call fce_load_rom(rom) for parsing
  4) call fce_init for emulator initialization
  5) call fce_run(), which is a non-exiting loop simulating the NES system
  6) when SIGINT signal is received, it kills itself
*/
#include "fce.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "key.h"
#include "main.h"

static char rom[1048576];

void do_exit() // normal exit at SIGINT
{
    kill(getpid(), SIGKILL);
}

int main(int argc, char *argv[])
{
#if USE_LCD_RTP
    if (argc != 5)
    {
        fprintf(stderr, "Usage: litenes romfile.nes ip port input\n");
        exit(1);
    }
#else
    if (argc != 4)
    {
        fprintf(stderr, "Usage: litenes romfile.nes fbd input\n");
        exit(1);
    }
#endif
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Open rom file failed.\n");
        exit(1);
    }
    int nread = fread(rom, sizeof(rom), 1, fp);
    if (nread == 0 && ferror(fp)) {
        fprintf(stderr, "Read rom file failed.\n");
        exit(1);
    }
    if (fce_load_rom(rom) != 0)
    {
        fprintf(stderr, "Invalid or unsupported rom.\n");
        exit(1);
    }
    signal(SIGINT, do_exit);
  #if USE_LCD_RTP
    lcd_init(argv[2],atoi(argv[3]));
    key_init(argv[4]);
  #else
    lcd_init(argv[2]);
    key_init(argv[3]);
  #endif
    fce_init();
    fce_run();
    return 0;
}
