#include "screen_util.h"

unsigned int pixel_color(unsigned char r, unsigned char g, unsigned char b) {
    return (r<<16) | (g<<8) | b;
}