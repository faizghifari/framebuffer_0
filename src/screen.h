#ifndef SCREEN_H
#define SCREEN_H

#include <linux/fb.h>

typedef struct {
    int fb_fd;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    unsigned char *fb_ptr, *bb_ptr;
} screen;

void init_screen(screen* scr_out);

screen create_screen(int width, int height);

void get_screen_width(screen* scr, unsigned int* width);

void get_screen_height(screen* scr, unsigned int* height);

int get_screen_pixel(screen* scr, unsigned int x, unsigned int y);

void put_pixel(screen* scr, unsigned int x, unsigned int y, unsigned int pixel);

void flush_screen(screen* scr);

void free_screen(screen* scr);

#endif