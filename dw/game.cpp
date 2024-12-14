#include "game.h"
#include "creature.h"

struct gamei {
	char		bolster;
};
static gamei game;

int party_average(abilityn v) {
	auto count = 0, total = 0;
	for(auto& e : bsdata<creature>()) {
		if(!e)
			continue;
		total += e.get(v);
		count += 1;
	}
	if(!count)
		return 0;
	return total / count;
}

int party_bolster() {
	return game.bolster;
}

void use_bolster() {
	game.bolster--;
}