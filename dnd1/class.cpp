#include "answers.h"
#include "avatarable.h"
#include "class.h"
#include "creature.h"
#include "crt.h"
#include "draw_utg.h"
#include "gender.h"

namespace draw {
extern int tab_pixels;
}

BSDATA(classi) = {
	{"Monster", MeleeToHit, {}, 0, 8},
	{"Cleric", Wisdow, {}, 1, 6},
	{"Dwarf", Strenght, {0, 0, 9, 0, 0, 0}, 0, 8},
	{"Elf", Dexterity, {0, 0, 0, 9, 0, 0}, 0, 6},
	{"Fighter", Strenght, {}, 0, 8},
	{"Halfling", Dexterity, {0, 9, 9, 0, 0, 0}, 0, 6},
	{"Theif", Dexterity, {}, 1, 4},
	{"Wizard", Intellect, {}, 2, 4},
};
assert_enum(classi, Wizard)

static avatarable last_avatar;

static void add_abilitites() {
	for(auto i = Strenght; i <= Charisma; i = (ability_s)(i + 1))
		answers::console->addn("  %2i\t%1", getnm(bsdata<abilityi>::elements[i].id), player->basic.abilities[i]);
}

static bool isallow(const char* minimal) {
	for(auto i = 0; i < 6; i++) {
		if(minimal[i] && player->basic.abilities[i] < minimal[i])
			return false;
	}
	return true;
}

static void add_adjust(answers& an, ability_s a) {
	auto& ei = bsdata<classi>::elements[player->type];
	if(ei.prime == a)
		return;
	if(player->basic.abilities[ei.prime] >= 18)
		return;
	an.add(bsdata<abilityi>::elements + a, getnm("IncreaseAndLower"),
		getnm(bsdata<abilityi>::elements[ei.prime].id),
		getnm(bsdata<abilityi>::elements[a].id));
}

static void choose_avatar() {
	player->setavatar(avatarable::choose(getnm("ChooseLook"), (player->gender == Female) ? "f*.*" : "m*.*", 6));
}

static void adjust_ability_scores() {
	answers an;
	auto push_tabs = draw::tab_pixels; draw::tab_pixels = 36;
	while(!draw::isnext()) {
		auto& ei = bsdata<classi>::elements[player->type]; an.clear();
		answers::console->clear();
		answers::console->addn(getnm("YouMayAdjustAbility"), getnm(ei.id));
		add_abilitites();
		add_adjust(an, Strenght);
		add_adjust(an, Intellect);
		add_adjust(an, Wisdow);
		auto pi = (abilityi*)an.choose(getnm("WhatToDoYou"), getnm("DoNotAdjust"));
		if(!pi)
			break;
		auto a = (ability_s)(pi - bsdata<abilityi>::elements);
		player->basic.abilities[ei.prime] += 1;
		player->basic.abilities[a] -= 2;
	}
	draw::tab_pixels = push_tabs;
	answers::console->clear();
}

static void choose_class() {
	answers an;
	auto push_tabs = draw::tab_pixels; draw::tab_pixels = 36;
	answers::console->clear();
	answers::console->addn(getnm("RollRandomAbilities"));
	add_abilitites();
	for(auto& e : bsdata<classi>()) {
		if(&e == bsdata<classi>::elements)
			continue;
		if(!isallow(e.minimal))
			continue;
		an.add(&e, getnm(e.id));
	}
	auto p = (classi*)an.choose(getnm("ChooseClass"));
	player->type = (class_s)(p - bsdata<classi>::elements);
	draw::tab_pixels = push_tabs;
	answers::console->clear();
}

static void roll_ability_scores() {
	char temp[6] = {};
	auto push_tabs = draw::tab_pixels; draw::tab_pixels = 36;
	while(!draw::isnext()) {
		player->basic.rollability();
		answers an;
		answers::console->clear();
		answers::console->addn(getnm("RollRandomAbilities"));
		add_abilitites();
		an.add(temp, getnm("RerollAbilityAgain"));
		if(!an.choose(getnm("WhatToDoYou"), getnm("TakeThisAbility")))
			break;
	}
	draw::tab_pixels = push_tabs;
	answers::console->clear();
}

void creature::generate() {
	auto push_player = player;
	auto push_header = answers::header; answers::header = getnm("CharacterGeneration");
	player = this;
	player->clear();
	roll_ability_scores();
	choose_class();
	adjust_ability_scores();
	choose_avatar();
	finish_creature();
	answers::header = push_header;
	player = push_player;
}