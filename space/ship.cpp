#include "crt.h"
#include "planet.h"
#include "ship.h"

long distance(point p1, point p2);

ship* last_ship;
ship* player;

int	ship::getspeed() const {
	return 32;
}

planeti* ship::gethomeworld() const {
	return getbs<planeti>(homeworld);
}

void ship::move(point position) {
	auto d = distance(this->position, position);
	auto n = d * 1000 / getspeed();
	drawable::move(position, n);
}