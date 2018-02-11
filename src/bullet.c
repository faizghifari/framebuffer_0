#include "bullet.h"

bullet draw_bullet_from_vector(float x, float y, float dx, float dy, float gravity){
	bullet b;

	float mag = sqrt(dx*dx + dy*dy);
    dx /= mag;
    dy /= mag;

	b.x = x;
	b.y = y;
	b.dx = dx;
	b.dy = dy;
	b.t = gravity;

	return b;
}

void move_bullet(bullet* b, float velocity) {
	(*b).dy -= (*b).t;

	(*b).x += (*b).dx * velocity;
	(*b).y += (*b).dy * velocity;
}

void draw_bullet(screen* scr, bullet b, float length) {
	draw_line(scr, b.x, b.y, b.x + b.dx * length, b.y + b.dy * length, 0xffff00);
}