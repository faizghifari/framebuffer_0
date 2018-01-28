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

const int MAX_BULLET = 20;
const float BULLET_VELOCITY = 10.0;
const float BULLET_LENGTH = 5.0;

typedef struct {
    float x, y, dx, dy;
} bullet;

void clear_screen(screen* scr);

int add_bullet_by_vector(int x, int y, float dx, float dy);

int n_bullet;
bullet* p_bullet;

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
    
    n_bullet = 0;
    p_bullet = (bullet*) malloc(MAX_BULLET * sizeof(bullet));

    add_bullet_by_vector(0, 100, 1, 0);

    int top = 0;
    while (1) {
        clear_screen(&scr);

        image img;
        load_image_from_file("data/pesawat.txt", &img);
        draw_image(&scr, 10, 10, 0xffffff, img);

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

        // render the bullet
        for (i = 0; i < n_bullet; i++) {
            bullet b = p_bullet[i];
            draw_line(&scr, b.x, b.y, b.x + b.dx * BULLET_VELOCITY, b.y + b.dy * BULLET_LENGTH);
        }

        if (rand() % 100 < 30)
            add_bullet_by_vector(rand() % width,
                                 rand() % height,
                                 rand() % (2*width) - width,
                                 rand() % (2*height) - height);

        flush_screen(&scr);
        usleep(1);
        top = (top + 3) % height;
        // move the bullet
        for (i = 0; i < n_bullet; i++) {
            p_bullet[i].x += p_bullet[i].dx * BULLET_VELOCITY;
            p_bullet[i].y += p_bullet[i].dy * BULLET_VELOCITY;
        }
        // remove bullet
        int removed = 0;
        for (i = 0; i < n_bullet; i++)
            if (p_bullet[i].x < 0 || p_bullet[i].x > width || p_bullet[i].y < 0 || p_bullet[i].y > height)
                removed++;
            else
                p_bullet[i - removed] = p_bullet[i];
        n_bullet -= removed;
    }

    free_screen(&scr);
    free(p_bullet);

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

int add_bullet_by_vector(int x, int y, float dx, float dy) {
    float mag = sqrt(dx*dx + dy*dy);
    dx /= mag;
    dy /= mag;

    if (n_bullet < MAX_BULLET) {
        bullet b;
        b.x = x;
        b.y = y;
        b.dx = dx;
        b.dy = dy;

        p_bullet[n_bullet++] = b;
        return n_bullet - 1;
    }
    return -1;
}