#include "screen.h"
#include "image.h"
#include "screen_util.h"

typedef struct {
    float x, y, dx, dy, t;
} bullet;

bullet draw_bullet_from_vector(float x, float y, float dx, float dy);

void move_bullet (bullet* b);