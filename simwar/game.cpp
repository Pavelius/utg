#include "game.h"

gamei game;

static void random_explore() {
	for(auto& e : bsdata<provincei>()) {
		auto size = e.landscape->effect[Size] + e.income[Size];
		auto minimum = size * 3;
		auto maximum = minimum + size * 10;
		e.explore(xrand(minimum, maximum));
	}
}

void gamei::initialize() {
	year = 1410;
	random_explore();
}