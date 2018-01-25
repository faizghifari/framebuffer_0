#ifndef SCREEN_UTIL_H
#define SCREEN_UTIL_H

#include "screen.h"

unsigned int pixel_color(unsigned char r, unsigned char g, unsigned char b);

void draw_line(screen* scr, int x1, int y1, int x2, int y2);

#endif