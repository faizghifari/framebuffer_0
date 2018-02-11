#include <math.h>

#include "screen.h"
#include "image.h"
#include "screen_util.h"

typedef struct {
    float x, y, dx, dy, t;
} bullet;

bullet add_bullet(float x, float y, float dx, float dy, float gravity);

void move_bullet (bullet* b, float velocity);

void draw_bullet (screen* src, bullet b, float length);