#include "screen_util.h"
#include <stdlib.h>

unsigned int pixel_color(unsigned char r, unsigned char g, unsigned char b) {
    return (r<<16) | (g<<8) | b;
}

void __draw_line_x(screen* scr, int x1, int y1, int x2, int y2) {
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
    int dy = abs(y2 - y1);
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

void __draw_line_y(screen* scr, int x1, int y1, int x2, int y2) {
    if (y1 > y2) {
        int t = y1;
        y1 = y2; 
        y2 = t;
        t = x1;
        x1 = x2;
        x2 = t;
    }
    char inc = x2 >= x1 ? 1 : 0;
    int dy = y2 - y1;
    int dx = abs(x2 - x1);
    int e = 0;
    for (;y1 <= y2; y1++) {
        if (e <= 0) {
            put_pixel(scr, x1, y1, 0xffffff);
            e = e + 2*dx;
        } else {
            put_pixel(scr, inc ? ++x1 : --x1, y1, 0xffffff);
            e = e + 2*dx - 2*dy;
        }
    }
}

void draw_line(screen* scr, int x1, int y1, int x2, int y2) {
    if (abs(x2 - x1) > abs(y2 - y1))
        __draw_line_x(scr, x1, y1, x2, y2);
    else
        __draw_line_y(scr, x1, y1, x2, y2);
}