#include "math.h"
#include "rand.h"
#include "statable.h"

static int ability_bonus[] = {
	-4, -4, -4, -3, -2, -2, -1, -1, -1, 0,
	0, 0, 0, 1, 1, 1, 2, 2, 3
};
static int ability_bonush[] = {
	-3, -3, -3, -2, -1, -1, -1, -1, -1, 0,
	0, 0, 0, 1, 1, 1, 1, 1, 2
};

int statable::getbonus(abilityn v) const {
	return maptbl(ability_bonus, get(v));
}

int statable::getbonush(abilityn v) const {
	return maptbl(ability_bonush, get(v));
}

void statable::rollability() {
	for(auto i = 0; i < 6; i++)
		abilities[i] = 3 + (rand() % 6) + (rand() % 6) + (rand() % 6);
}

abilityn statable::getbestability() const {
	auto r = 0;
	auto m = 0;
	for(auto i = 0; i < 6; i++) {
		if(abilities[i] > m) {
			r = i;
			m = abilities[i];
		}
	}
	return (abilityn)r;
}

void statable::applybest(abilityn v) {
	auto m = getbestability();
	if(v == m)
		return;
	iswap(abilities[v], abilities[m]);
}