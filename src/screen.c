#include "screen.h"
#include <linux/fb.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

void init_screen(screen* scr) {
    if (scr == NULL)
        return;
    screen sc;

    sc.fb_fd = open("/dev/fb0",O_RDWR);

    ioctl(sc.fb_fd, FBIOGET_VSCREENINFO, sc.vinfo);
    sc.vinfo.grayscale=0;
    sc.vinfo.bits_per_pixel=32;

    ioctl(sc.fb_fd, FBIOPUT_VSCREENINFO, &sc.vinfo);
    ioctl(sc.fb_fd, FBIOGET_VSCREENINFO, &sc.vinfo);
    ioctl(sc.fb_fd, FBIOGET_FSCREENINFO, &sc.finfo);

    long screensize = sc.vinfo.yres_virtual * sc.finfo.line_length;
    sc.fb_ptr = mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, sc.fb_fd, (off_t) 0);
    sc.bb_ptr = (unsigned char*) malloc(sc.vinfo.yres_virtual * sc.finfo.line_length);

    memcpy(scr, &sc, sizeof(sc));
}

screen create_screen(int width, int height) {
    screen sc;
    sc.vinfo.bits_per_pixel = sizeof(int) * 8;
    sc.vinfo.yres = height;
    sc.finfo.line_length = width * sizeof(int);
    sc.vinfo.xoffset = 0;
    sc.vinfo.yoffset = 0;
    sc.bb_ptr = (char*) malloc(sizeof(int) * width * height);
    memset(sc.bb_ptr, 0, sizeof(int) * width * height);

    return sc;
}

void get_screen_width(screen* scr, unsigned int* width) {
    if (width == NULL)
        return;
    if (scr == NULL) {
        *width = 0;
        return;
    }
    *width = scr->finfo.line_length / ((scr->vinfo.bits_per_pixel + 7) / 8);
}

void get_screen_height(screen* scr, unsigned int* height) {
    if (height == NULL)
        return;
    if (scr == NULL) {
        *height = 0;
        return;
    }
    *height = scr->vinfo.yres_virtual;
}

void put_pixel(screen* scr, unsigned int x, unsigned int y, unsigned int pixel) {
    if (scr == NULL)
        return;
    int width, height;
    get_screen_height(scr, &height);
    get_screen_width(scr, &width);
    if (x >= width || y >= height)
        return;
        
    long mem_loc = (x + scr->vinfo.xoffset) * (scr->vinfo.bits_per_pixel/8);
    mem_loc += (y + scr->vinfo.yoffset) * scr->finfo.line_length;
    *((unsigned int*) (scr->bb_ptr + mem_loc)) = pixel;
}

void flush_screen(screen* scr) {
    if (scr == NULL)
        return;
    memcpy(scr->fb_ptr, scr->bb_ptr, scr->vinfo.yres_virtual * scr->finfo.line_length);
}

void free_screen(screen* scr) {
    if (scr == NULL)
        return;
    long screensize = scr->vinfo.yres_virtual * scr->finfo.line_length;
    munmap(scr->fb_ptr, screensize);
    free(scr->bb_ptr);
}