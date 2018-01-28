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

const int MAX_BULLET = 50;
const float BULLET_VELOCITY = 10.0;
const float BULLET_LENGTH = 5.0;

const int MAX_PLANE = 20;
const float PLANCE_VELOCITY = 5.0;

const int MAX_COLLISION_TIME = 10;
const int COLLISION_DETAIL = 9;

typedef struct {
    float x, y, dx, dy;
} bullet;

typedef struct {
    int x, y, dx;
} plane;

typedef struct {
    int x, y, time;
} collision;

void clear_screen(screen* scr);

int add_bullet_by_vector(int x, int y, float dx, float dy);

int add_plane_by_vector(int x, int y, int dx);

int add_collision(int x, int y);

int n_bullet;
int n_plane;
int n_collision;
bullet* p_bullet;
plane* p_plane;
collision* p_collision;

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
    n_collision = 0;

    p_bullet = (bullet*) malloc(MAX_BULLET * sizeof(bullet));
    p_plane = (plane*) malloc(MAX_PLANE * sizeof(plane));
    p_collision = (collision*) malloc(MAX_PLANE * MAX_COLLISION_TIME * sizeof(collision));

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
            draw_line(&scr, b.x, b.y, b.x + b.dx * BULLET_LENGTH, b.y + b.dy * BULLET_LENGTH, 0xffff00);
        }
        // render planes
        for (i = 0; i < n_plane; i++)
            draw_image(&scr, p_plane[i].x, p_plane[i].y, 0xffffff, plane_img);

        // render collision
        for (i = 0; i < n_collision; i++) {
            float angle = 2.0 * acos(-1) / COLLISION_DETAIL;
            for (int j = 0; j < COLLISION_DETAIL; j++) {
                float _y = sin(angle * j);
                float _x = cos(angle * j);
                
                float x0, y0, x1, y1;
                x0 = p_collision[i].x + _x * p_collision[i].time * 2;
                y0 = p_collision[i].y + _y * p_collision[i].time * 2;
                x1 = x0 + _x * p_collision[i].time;
                y1 = y0 + _y * p_collision[i].time;

                draw_line(&scr, x0, y0, x1, y1, 0xff0000);
            }
        }

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

        // update collision
        removed = 0;
        for (i = 0; i < n_collision; i++)
            if (p_collision[i].time > MAX_COLLISION_TIME)
                removed++;
            else {
                p_collision[i].time++;
                p_collision[i-removed] = p_collision[i];
            }
        n_collision -= removed;

        // add random bullet
        if (rand() % 100 < 70)
            add_bullet_by_vector(width/2,
                                 height,
                                 rand() % (2*width) - width,
                                 rand() % (2*height) - height);

        // add random plane
        if (rand() % 100 < 30)
            add_plane_by_vector(width, rand() % height, -1);

        // detect collision
        int j;
        for (i = 0; i < n_bullet; i++) {
            removed = 0;
            for (j = 0; j < n_plane; j++)
                if (p_bullet[i].x >= p_plane[j].x && p_bullet[i].x <= p_plane[j].x + 60 &&
                    p_bullet[i].y > p_plane[j].y && p_bullet[i].y < p_plane[j].y + 30) {
                    add_collision(p_plane[j].x + 30, p_plane[j].y + 15);
                    removed++;
                } else
                    p_plane[j - removed] = p_plane[j];
            n_plane -= removed;
        }
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

int add_collision(int x, int y) {
    collision c;
    c.x = x;
    c.y = y;
    c.time = 0;
    p_collision[n_collision++] = c;
    return n_collision - 1;
}