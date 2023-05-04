#include "creature.h"
#include "ongoing.h"
#include "modifier.h"
#include "monster.h"
#include "pushvalue.h"
#include "script.h"

creature* player;

static void wearing(variant v) {
	if(v.iskind<abilityi>())
		player->abilities[v.value] += v.counter;
}

static void wearing(const variants& source) {
	for(auto v : source)
		wearing(v);
}

static void advance(variants source) {
	pushvalue push_modifier(modifier, Permanent);
	for(auto v : source)
		script::run(v);
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

static void update_maximum() {
	for(auto i = Strenght; i <= Charisma; i = (ability_s)(i + 1)) {
		if(player->abilities[i] > 18)
			player->abilities[i] = 18;
	}
}

void creature::clear() {
	memset(this, 0, sizeof(*this));
}

void creature::update() {
	pushvalue push(player, this);
	update_start();
	update_equipment();
	update_ongoing();
	update_maximum();
}

bool creature::isallow(const item& v) const {
	auto& ei = v.geti();
	switch(ei.wear) {
	case MeleeWeapon: case RangedWeapon: case Torso:
		return !usable || (usable & FG(getbsi(&ei))) != 0;
	default:
		return true;
	}
}

static int roll(int value, int advantages) {
	auto r = xrand(1, value);
	if(advantages > 0) {
		auto r1 = xrand(1, value);
		if(r1 > r)
			r = r1;
	} else if(advantages < 0) {
		auto r1 = xrand(1, value);
		if(r1 < r)
			r = r1;
	}
	return r;
}

static void roll_hit_points() {
	auto hits = bsdata<classi>::elements[player->kind].hits;
	auto result = roll(hits, player->is(BonusHitRoll) ? 1 : 0);
	player->basic.abilities[HP] += result;
}

static void roll_random_abilities() {
	while(true) {
		player->basic.rollability();
		auto n = player->basic.getbestability();
		if(player->basic.get(n) >= 14)
			break;
	}
}

static void copy_special_abilities(const statable& e) {
	for(auto i = HP; i <= Dodge; i = (ability_s)(i + 1))
		player->basic.abilities[i] = e.abilities[i];
}

static void transform_ability() {
	for(auto i = Strenght; i <= Charisma; i = (ability_s)(i + 1))
		player->basic.abilities[i] = (player->basic.abilities[i] + 5) * 2 + xrand(0, 1);
}

static void starting_hits() {
	player->basic.abilities[HP] += player->basic.getbonus(Constitution);
	if(player->basic.abilities[HP] < 1)
		player->basic.abilities[HP] = 1;
	if(player->is(BonusHitRoll))
		player->basic.abilities[HP] += 2;
}

static void finish_create() {
	player->update();
	for(auto i = HP; i <= Dodge; i = (ability_s)(i + 1))
		player->abilities[i] = player->basic.abilities[i];
}

void creature::add(race_s race, class_s kind) {
	player = bsdata<creature>::add();
	player->clear();
	player->ancestry = race;
	player->kind = kind;
	roll_random_abilities();
	roll_hit_points();
	starting_hits();
	finish_create();
}

void creature::add(const char* id) {
	add(bsdata<monsteri>::find(id));
}

void creature::add(const monsteri* pm) {
	if(!pm)
		return;
	player = bsdata<creature>::add();
	player->clear();
	player->basic.load(*pm);
	transform_ability();
	copy_special_abilities(*pm);
	advance(pm->feats);
	finish_create();
}