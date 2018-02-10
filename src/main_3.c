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

void put_char_to_screen(screen* scr, char c, int x, int y);

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: main <string-to-show>");
        return -1;
    }
    char* message = argv[1];

    screen scr;
    init_screen(&scr);
    int width, height;
    get_screen_height(&scr, &height);
    get_screen_width(&scr, &width);

    int top = 0;
    while (1) {
        clear_screen(&scr);

        int i;
        for (i = 0; i < (int) strlen(message); i++)
            put_char_to_screen(&scr, message[i], 10 + i * 25, 10);
        flush_screen(&scr);

        usleep(10);
    }

    free_screen(&scr);

    return 0;
}

void put_char_to_screen(screen* scr, char c, int x, int y) {
    char fname[256];
    sprintf(fname, "data/%d.txt", (int) c);
    
    if (c == 'J' || c == 'R' || c == 'A' || c == 'F' || c == 'I' || c == 'K')
        sprintf(fname, "data/%c.txt", c);

    image i_image;
    load_image_from_file(fname, &i_image);
    draw_image(scr, x, y, 0xffffff, i_image);
}