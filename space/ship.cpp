#include "assign.h"
#include "crt.h"
#include "planet.h"
#include "pushvalue.h"
#include "ship.h"
#include "timeable.h"

BSDATA(shipclassi) = {
	{"Fighter", 100, 20, 0, 0},
	{"Fregate", 200, 15, 0, 10},
	{"Destroyer", 250, 20, 0, 0},
	{"Cruiser", 300, 15, 1, 5},
	{"Battleship", 400, 10, 2, 0},
	{"Carrier", 400, 5}, // Big transporter
};
assert_enum(shipclassi, Carrier)

long distance(point p1, point p2);

ship* last_ship;
ship* player;

const shipclassi& shipi::geti() const {
	return bsdata<shipclassi>::elements[kind];
}

long ship::rangeto(const ship& v) const {
	return distance(position, v.position);
}

bool ship::cansee(const ship& v) const {
	return rangeto(v) <= modules[Sensors];
}

const shipi& ship::geti() const {
	return bsdata<shipi>::elements[type];
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

static void update_stats() {
	auto& ei = last_ship->geti(); auto& ec = ei.geti();
	last_ship->modules[Hull] = ec.hull * (ei.size + last_ship->modules[Hull]);
	last_ship->modules[Engine] = ec.speed * (2 + last_ship->modules[Engine]); // Pixels per day
	last_ship->modules[Sensors] = 30 * (2 + last_ship->modules[Sensors]); // Pixels for radar
	last_ship->modules[Shield] = ec.shield + 15 * last_ship->modules[Shield];
	last_ship->modules[Armor] = ec.armor + last_ship->modules[Armor];
}

void ship::update_correction() {
	if(modules[Hull] > hull)
		hull = modules[Hull];
	if(modules[Shield] > shield)
		shield = modules[Shield];
}

void ship::update() {
	pushvalue push(last_ship, this);
	assign<modulea>(*this, basic);
	update_stats();
	update_correction();
}

void ship::recover() {
	if(modules[Shield])
		shield += 2;
	update();
}