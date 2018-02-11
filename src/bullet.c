#include "bullet.h"

bullet add_bullet(float x, float y, float dx, float dy, float velocity, float gravity){
	bullet b;

	float mag = sqrt(dx*dx + dy*dy);
    dx /= mag;
    dy /= mag;

	b.x = x;
	b.y = y;
	b.dx = dx * velocity;
	b.dy = dy * velocity;
	b.t = gravity;

	return b;
}

void move_bullet(bullet* b, float velocity) {
	(*b).dy += (*b).t;

	(*b).x += (*b).dx * velocity;
	(*b).y += (*b).dy * velocity;
}

void draw_bullet(screen* scr, bullet b, float length) {
	double _mag = sqrt(b.dx*b.dx + b.dy*b.dy);
	double _dx = b.dx / _mag;
	double _dy = b.dy / _mag;
	draw_line(scr, b.x, b.y, b.x + _dx * length, b.y + _dy * length, 0xffff00);
}