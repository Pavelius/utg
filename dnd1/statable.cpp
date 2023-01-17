#include "crt.h"
#include "statable.h"

static unsigned fighter_experience[] = {
	0, 0, 2000, 4000, 8000, 16000, 32000, 64000, 120000, 240000,
	360000, 480000, 600000, 720000, 840000,
};
static unsigned elf_experience[] = {
	0, 0, 4000, 8000, 16000, 32000, 64000, 120000, 250000, 400000,
	480000, 600000,
};
static unsigned dwarf_experience[] = {
	0, 0, 2200, 4400, 8800, 17000, 35000, 70000, 140000, 270000,
	400000, 530000, 660000,
};
static char cleric_saves[][5] = {
	{11, 12, 14, 16, 15},
	{11, 12, 14, 16, 15},
	{11, 12, 14, 16, 15},
	{11, 12, 14, 16, 15},
	{11, 12, 14, 16, 15},
	{9, 10, 12, 14, 12},
	{9, 10, 12, 14, 12},
	{9, 10, 12, 14, 12},
	{9, 10, 12, 14, 12},
	{6, 7, 9, 11, 9},
	{6, 7, 9, 11, 9},
	{6, 7, 9, 11, 9},
	{6, 7, 9, 11, 9},
	{3, 5, 7, 8, 7},
};
static char fighter_saves[][5] = {
	{12, 13, 14, 15, 16},
	{12, 13, 14, 15, 16},
	{12, 13, 14, 15, 16},
	{10, 11, 12, 13, 14},
	{10, 11, 12, 13, 14},
	{10, 11, 12, 13, 14},
	{8, 9, 10, 10, 12},
	{8, 9, 10, 10, 12},
	{8, 9, 10, 10, 12},
	{6, 7, 8, 8, 10},
	{6, 7, 8, 8, 10},
	{6, 7, 8, 8, 10},
	{4, 5, 6, 5, 8},
};
static int ability_bonus[] = {
	-4, -4, -4, -3, -2, -2, -1, -1, -1, 0,
	0, 0, 0, 1, 1, 1, 2, 2, 3
};
static int ability_bonush[] = {
	-3, -3, -3, -2, -1, -1, -1, -1, -1, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 2
};
static int attack_bonus[3][15] = {
	{-1, 0, 0, 0, 2, 2, 2, 5, 5, 5, 7, 7, 7, 9, 9},
	{-1, 0, 0, 0, 0, 2, 2, 2, 2, 5, 5, 5, 5, 7, 7},
	{-1, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 5, 5, 5, 5},
};

int statable::getattackbonus(int type, int level) {
	return maptbl(attack_bonus[type], level);
}

int statable::getbonus(ability_s v) const {
	return maptbl(ability_bonus, get(v));
}

int statable::getbonush(ability_s v) const {
	return maptbl(ability_bonush, get(v));
}

unsigned statable::getexperience(class_s type, int level) {
	switch(type) {
	case Elf: return maptbl(elf_experience, level);
	case Dwarf: return maptbl(dwarf_experience, level);
	default: return maptbl(fighter_experience, level);
	}
}

char statable::getsave(class_s type, ability_s save, int level) {
	if(save == SavePoison)
		save = SaveDeath;
	switch(type) {
	case Cleric: return maptbl(cleric_saves, level)[save - SaveDeath];
	default: return maptbl(cleric_saves, level)[save - SaveDeath];
	}
}

void statable::rollability() {
	for(auto i = 0; i < 6; i++)
		abilities[i] = 3 + (rand() % 6) + (rand() % 6) + (rand() % 6);
}

void statable::applyminimal(class_s v) {
	auto& ei = bsdata<classi>::elements[v];
	for(auto i = 0; i < 6; i++) {
		if(abilities[i] < ei.minimal[i])
			abilities[i] = ei.minimal[i];
	}
}

ability_s statable::getbestability() const {
	auto r = 0;
	auto m = 0;
	for(auto i = 0; i < 6; i++) {
		if(abilities[i] > m) {
			r = i;
			m = abilities[i];
		}
	}
	return (ability_s)r;
}

void statable::applybest(ability_s v) {
	auto m = getbestability();
	if(v == m)
		return;
	iswap(abilities[v], abilities[m]);
}