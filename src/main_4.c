#include <linux/fb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <termios.h>

#include "screen.h"
#include "image.h"
#include "screen_util.h"

void clear_screen(screen* scr);

int main(int argc, char** argv) {
    screen scr;
    init_screen(&scr);
    int width, height;
    get_screen_height(&scr, &height);
    get_screen_width(&scr, &width);

    while (1) {
        clear_screen(&scr);
        flush_screen(&scr);
        usleep(10);
    }

    free_screen(&scr);

    return 0;
}

void clear_screen(screen* scr){
    int x, y, width, height;
    get_screen_height(scr, &height);
    get_screen_width(scr, &width);
    
    for (x = 0; x < width; x++)
        for (y = 0; y < height; y++)
            put_pixel(scr, x, y, 0);
}