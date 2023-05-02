#include "creature.h"
#include "ongoing.h"
#include "pushvalue.h"

creature* player;

static int get_weapon_flag(const itemi& ei) {
	return &ei - item_weapon;
}

static int get_armor_flag(const itemi& ei) {
	return &ei - item_armor;
}

static bool is_weapon(const itemi& ei) {
	return ei.wear == MeleeWeapon || ei.wear == RangedWeapon;
}

static void wearing(variant v) {
	if(v.iskind<abilityi>())
		player->abilities[v.value] += v.counter;
}

static void wearing(const variants& source) {
	for(auto v : source)
		wearing(v);
}

static void update_start() {
	player->load(player->basic);
}

static void update_equipment() {
	for(auto& e : player->equipment()) {
		if(e)
			wearing(e.geti().wearing);
	}
}

static void update_ongoing() {
	variant owner = player;
	for(auto& e : bsdata<ongoing>()) {
		if(e.owner == owner)
			wearing(e.effect);
	}
}

void creature::update() {
	pushvalue push(player, this);
	update_start();
	update_equipment();
	update_ongoing();
}

bool creature::isallow(const item& v) const {
	auto& ei = v.geti();
	switch(ei.wear) {
	case MeleeWeapon: case RangedWeapon:
		return !weapons || (weapons & FG(get_weapon_flag(ei))) != 0;
	case Torso:
		return !armors || (armors & FG(get_armor_flag(ei))) != 0;
	default:
		return true;
	}
}