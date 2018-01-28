#ifndef SCREEN_UTIL_H
#define SCREEN_UTIL_H

#include "screen.h"
#include "image.h"

unsigned int pixel_color(unsigned char r, unsigned char g, unsigned char b);

void draw_line(screen* scr, int x1, int y1, int x2, int y2, int color);

void draw_image(screen* scr, int x, int y, int initial_color, image img);

#endif