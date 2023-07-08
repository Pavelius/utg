#include "crt.h"
#include "planet.h"
#include "ship.h"
#include "timeable.h"

long distance(point p1, point p2);

ship* last_ship;
ship* player;

int	ship::getspeed() const {
	return 40;
}

planeti* ship::gethomeworld() const {
	return getbs<planeti>(homeworld);
}

planeti* ship::getplanet() const {
	auto system_id = getbsi(last_system);
	for(auto& e : bsdata<planeti>()) {
		if(e.system == system_id && e.position == position)
			return &e;
	}
	return 0;
}

void ship::move(point position) {
	auto d = distance(this->position, position);
	auto n = d * timeable::rday / getspeed();
	moveable::move(position, n);
}