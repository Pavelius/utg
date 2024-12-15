#include "assign.h"
#include "bsdata.h"
#include "planet.h"
#include "pushvalue.h"
#include "ship.h"
#include "timeable.h"

BSDATA(shipclassi) = {
	{"Fighter", 3, 100, 20, 0, 0},
	{"Fregate", 4, 200, 15, 0, 10},
	{"Destroyer", 4, 250, 20, 0, 0},
	{"Cruiser", 5, 300, 15, 1, 5},
	{"Battleship", 6, 400, 10, 2, 0},
	{"Carrier", 6, 400, 5}, // Big transporter
};
assert_enum(shipclassi, Carrier)

long distance(point p1, point p2);

ship* player;

const shipclassi& shipi::geti() const {
	return bsdata<shipclassi>::elements[kind];
}

int	ship::getpixelsize() const {
	return geti().geti().size + basic.modules[Hull];
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
	auto& ei = player->geti(); auto& ec = ei.geti();
	player->modules[Hull] = ec.hull + player->modules[Hull] * 10;
	player->modules[Engine] = ec.speed * (2 + player->modules[Engine]); // Pixels per day
	player->modules[Sensors] = 30 * (2 + player->modules[Sensors]); // Pixels for radar
	player->modules[Shield] = ec.shield + 15 * player->modules[Shield];
	player->modules[Armor] = ec.armor + player->modules[Armor];
}

void ship::update_correction() {
	if(modules[Hull] > hull)
		hull = modules[Hull];
	if(modules[Shield] > shield)
		shield = modules[Shield];
}

void ship::update() {
	pushvalue push(player, this);
	assign<modulea>(*this, basic);
	update_stats();
	update_correction();
}

void ship::recover() {
	if(modules[Shield])
		shield += 2;
	update();
}