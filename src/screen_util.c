#include "screen_util.h"

unsigned int pixel_color(unsigned char r, unsigned char g, unsigned char b) {
    return (r<<16) | (g<<8) | b;
}

void draw_line(screen* scr, int x1, int y1, int x2, int y2) {
    if (x1 > x2) {
        int t = x1;
        x1 = x2; 
        x2 = t;
        t = y1;
        y1 = y2;
        y2 = t;
    }
    char inc = y2 >= y1 ? 1 : 0;
    int dx = x2 - x1;
    int dy = y2 - y1;
    int e = 0;
    for (;x1 <= x2; x1++) {
        if (e <= 0) {
            put_pixel(scr, x1, y1, 0xffffff);
            e = e + 2*dy;
        } else {
            put_pixel(scr, x1, inc ? ++y1 : --y1, 0xffffff);
            e = e + 2*dy - 2*dx;
        }
    }
}