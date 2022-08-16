#include "main.h"

static int thac0[3][15] = {
	{20, 19, 19, 19, 17, 17, 17, 14, 14, 14, 12, 12, 12, 10, 10},
	{20, 19, 19, 19, 19, 17, 17, 17, 17, 14, 14, 14, 14, 12, 12},
	{20, 19, 19, 19, 19, 19, 17, 17, 17, 17, 17, 14, 14, 14, 14}
};
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

int	creature::gethit() const {
	auto n = imin(14, get(Level));
	return thac0[n][bsdata<classi>::elements[type].tohit] + get(ToHit);
}

static unsigned getexperience(unsigned* table, int level) {
	return table[level];
}

void creature::raiselevel() {
	auto n = get(Level);
	auto d = bsdata<classi>::elements[type].hd;
}

void creature::levelup() {
	while(getexperience(fighter_experience, get(Level)) <= experience) {
		add(Level, 1);
		raiselevel();
	}
}

bool creature::attack(int bonus, int ac) const {
	auto d = 1 + rand() % 20;
	auto r = d + bonus;
	auto f = gethit() - ac;
	if(d == 1)
		return false;
	if(d == 20)
		return true;
	return r >= f;
}