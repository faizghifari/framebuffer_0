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
const float BULLET_VELOCITY = 50.0;
const float BULLET_LENGTH = 5.0;

const int MAX_PLANE = 5;
const float PLANCE_VELOCITY = 25.0;

typedef struct {
    float x, y, dx, dy;
} bullet;

typedef struct {
    float x, y, dx;
} plane;

void clear_screen(screen* scr);

int add_bullet_by_vector(int x, int y, float dx, float dy);

int add_plane_by_vector(int x, int y, int dx);

int n_bullet;
int n_plane;
bullet* p_bullet;
plane* p_plane;

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
    n_plane = 0;

    p_bullet = (bullet*) malloc(MAX_BULLET * sizeof(bullet));
    p_plane = (plane*) malloc(MAX_PLANE * sizeof(plane));

    image plane_img;
    load_image_from_file("data/pesawat.txt", &plane_img);

    int top = 0;
    while (1) {
        clear_screen(&scr);

        // render message
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
            draw_line(&scr, b.x, b.y, b.x + b.dx * BULLET_LENGTH, b.y + b.dy * BULLET_LENGTH);
        }
        // render planes
        for (i = 0; i < n_plane; i++)
            draw_image(&scr, p_plane[i].x, p_plane[i].y, 0xffffff, plane_img);

        flush_screen(&scr);
        usleep(10);

        // move message
        top = (top + 3) % height;

        // move the bullet
        for (i = 0; i < n_bullet; i++) {
            p_bullet[i].x += p_bullet[i].dx * BULLET_VELOCITY;
            p_bullet[i].y += p_bullet[i].dy * BULLET_VELOCITY;
        }
        // move the plane
        for (i = 0; i < n_plane; i++)
            p_plane[i].x += p_plane[i].dx * PLANCE_VELOCITY;

        // remove bullet
        int removed = 0;
        for (i = 0; i < n_bullet; i++)
            if (p_bullet[i].x < 0 || p_bullet[i].x > width || p_bullet[i].y < 0 || p_bullet[i].y > height)
                removed++;
            else
                p_bullet[i - removed] = p_bullet[i];
        n_bullet -= removed;

        // remove plane
        removed = 0;
        for (i = 0; i < n_plane; i++)
            if (p_plane[i].x < 0 || p_plane[i].x > width)
                removed++;
            else
                p_plane[i - removed] = p_plane[i];
        n_plane -= removed;

        // add random bullet
        if (rand() % 100 < 30)
            add_bullet_by_vector(rand() % width,
                                 rand() % height,
                                 rand() % (2*width) - width,
                                 rand() % (2*height) - height);

        // add random plane
        if (rand() % 100 < 30)
            add_plane_by_vector(width, rand() % height, -1);
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

int add_plane_by_vector(int x, int y, int dx) {
    float mag = dx > 0 ? 1 : -1;

    if (n_plane < MAX_PLANE) {
        plane p;
        p.x = x;
        p.y = y;
        p.dx = dx;

        p_plane[n_plane++] = p;
        return n_plane - 1;
    }
    return -1;
}