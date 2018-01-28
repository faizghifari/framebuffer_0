#include <linux/fb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include "screen.h"
#include "image.h"
#include "screen_util.h"

void clear_screen(screen* scr);

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
        int msg_n = strlen(message);
        for (i = 0; i < msg_n; i++) {
            char c = message[i];
            char filename[256];
            sprintf(filename, "data/%d.txt", (int) c);
            
            image img;
            if (load_image_from_file(filename, &img) > -1)
                draw_image(&scr, (width - msg_n * 20)/2 + 20*i, top, 0xffffff, img);
        }

        flush_screen(&scr);
        usleep(1);
        top = (top + 3) % height;
    }

    // while (1) {
    //     draw_line(&scr, 100, 100, 800, 150); // top left -> bottom right
    //     draw_line(&scr, 800, 160, 100, 110); // bottom right -> top left
    //     draw_line(&scr, 900, 100, 1600, 50); // bottom left -> top right
    //     draw_line(&scr, 1600, 60, 900, 110); // top right -> bottom left
    //     draw_line(&scr, 400, 200, 1200, 200); // left -> right
    //     draw_line(&scr, 1200, 210, 400, 210); // right -> left

    //     draw_line(&scr, 100, 300, 200, 600); // top left -> bottom right
    //     draw_line(&scr, 210, 600, 110, 300); // bottom right -> top left
    //     draw_line(&scr, 300, 300, 200, 600); // top right -> bottom left
    //     draw_line(&scr, 260, 600, 360, 300); // bottom left -> top right
    //     draw_line(&scr, 270, 300, 270, 600); // top -> bottom
    //     draw_line(&scr, 280, 600, 280, 300); // bottom -> top
        
    //     flush_screen(&scr);
    //     usleep(1);
    // }
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