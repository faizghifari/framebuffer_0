#include "screen_util.h"

unsigned int pixel_color(unsigned char r, unsigned char g, unsigned char b) {
    return (r<<16) | (g<<8) | b;
}

void draw_line(screen* scr, int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int e = 0;
    for (;x1 <= x2; x1++) {
        if (e <= 0) {
            put_pixel(scr, x1, y1, 0xffffff);
            e = e + 2*dy;
        } else {
            put_pixel(scr, x1, ++y1, 0xffffff);
            e = e + 2*dy - 2*dx;
        }
    }
}