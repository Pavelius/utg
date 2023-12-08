#include "ability.h"
#include "action.h"
#include "answers.h"
#include "creature.h"
#include "list.h"
#include "modifier.h"
#include "pushvalue.h"
#include "script.h"

static void* last_answer;
static int last_roll;

template<> void fnscript<modifieri>(int value, int bonus) {
	modifier = (modifier_s)value;
}

template<> void fnscript<weari>(int value, int bonus) {
	last_wear = (wear_s)value;
}
template<> bool fntest<weari>(int value, int bonus) {
	fnscript<weari>(value, bonus);
	return true;
}

static void reduce_ability(char& source, int& bonus) {
	if(!source || !bonus)
		return;
	if(source >= -bonus) {
		source += bonus;
		bonus = 0;
	} else {
		bonus += source;
		source = 0;
	}
}

static void reduce_ability(int value, int bonus) {
	switch(value) {
	case SwiftAction:
		reduce_ability(player->abilities[SwiftAction], bonus);
		reduce_ability(player->abilities[MoveAction], bonus);
		reduce_ability(player->abilities[StandartAction], bonus);
		break;
	case MoveAction:
		reduce_ability(player->abilities[MoveAction], bonus);
		reduce_ability(player->abilities[StandartAction], bonus);
		break;
	default:
		reduce_ability(player->abilities[value], bonus);
		break;
	}
}

static void add_ability(int value, int bonus) {
	if(bonus < 0)
		reduce_ability(value, bonus);
	else
		player->abilities[value] += bonus;
}

template<> void fnscript<abilityi>(int value, int bonus) {
	last_ability = (ability_s)value;
	switch(modifier) {
	case Permanent: player->basic.abilities[value] += bonus; break;
	default: add_ability(value, bonus); break;
	}
}

static int roll20() {
	return 1 + rand() % 20;
}

void roll20(int bonus) {
	auto n = roll20();
	last_roll = roll20() + bonus;
	answers::console->add("[{%1i%+2i=%3i}]", n, bonus, last_roll);
}

static bool allow_combat() {
	for(auto p : creatures) {
		if(p->is(Hostile))
			return true;
	}
	return false;
}

static void choose_creature(int bonus) {
	player = creatures.choose(0, getnm("Cancel"));
}

static void choose_opponent(int bonus) {
	opponent = creatures.choose(0);
}

static bool if_hands_item(int bonus) {
	if(player->hands != last_wear)
		return false;
	return player->wears[player->hands].operator bool();
}
static void hands_item(int bonus) {
	last_item = player->wears + player->hands;
}

static bool if_unarmed(int bonus) {
	return player->hands == MeleeWeapon && !player->wears[MeleeWeapon].operator bool();
}
static void ready_unarmed(int bonus) {
	last_item = 0;
}

static void make_melee_attack(int bonus) {
}

static void make_range_attack(int bonus) {
}

static void add_actions() {
	for(auto& e : bsdata<actioni>()) {
		if(e.upgrade) {
			if(e.upgrade.iskind<feati>() && player->isfeat(e.upgrade.value))
				continue;
		}
		if(!script_allow(e.effect))
			continue;
		an.add(&e, e.getprompt());
		if(an.getcount() > 16)
			break;
	}
}

static void ask_answer() {
	last_answer = (void*)an.choose(getnm("WhatYouDo"));
}

static void apply_answers() {
	if(bsdata<actioni>::have(last_answer))
		script_run(((actioni*)last_answer)->effect);
}

static void make_actions() {
	an.clear();
	add_actions();
	ask_answer();
	apply_answers();
}

void one_combat_round() {
	pushvalue push(player);
	for(auto p : creatures) {
		player = p;
		make_actions();
	}
}

BSDATA(script) = {
	{"ChooseCreature", choose_creature},
	{"ChooseOpponent", choose_opponent},
	{"HandsItem", hands_item, if_hands_item},
	{"ReadyUnarmed", ready_unarmed, if_unarmed},
};
BSDATAF(script)