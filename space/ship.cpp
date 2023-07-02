#include "crt.h"
#include "ship.h"

long distance(point p1, point p2);

ship* last_ship;

void ship::add(const char* id, point position) {
	auto pi = bsdata<shipi>::find(id);
	if(!pi)
		return;
	last_ship = bsdata<ship>::addz();
	last_ship->type = (unsigned char)getbsi(pi);
	last_ship->position = position;
	last_ship->priority = 2;
	last_ship->alpha = 255;
}

int	ship::getspeed() const {
	return 32;
}

void ship::move(point position) {
	auto d = distance(this->position, position);
	auto n = d * 1000 / getspeed();
	drawable::move(position, n);
}