#include "draw_hexagon.h"
#include "main.h"

using namespace pathfind;

gamei game;

duration_s gamei::getduration(variants source) {
	if(source) {
		auto v = source.begin();
		if(v->iskind<durationi>())
			return (duration_s)v->value;
	}
	return Instant;
}

int gamei::getrounds(variants source) {
	if(source) {
		auto v = source.begin();
		if(v->iskind<durationi>() && v->value==Use)
			return v->counter;
	}
	return 0;
}

static point getdirection(point hex, int direction) {
	static point evenr_directions[2][6] = {
		{{+1, 0}, {1, -1}, {0, -1}, {-1, 0}, {0, +1}, {+1, +1}},
		{{+1, 0}, {0, -1}, {-1, -1}, {-1, 0}, {-1, +1}, {0, +1}},
	};
	auto parity = hex.y & 1;
	auto offset = evenr_directions[parity][direction];
	return hex + offset;
}

static indext getdirection(indext index, int direction) {
	if(index == Blocked)
		return Blocked;
	auto hex = getdirection(i2h(index), direction);
	if(hex.x < 0 || hex.y < 0 || hex.x >= hms || hex.y >= hms)
		return Blocked;
	return h2i(hex);
}

static void pathfind_initialize() {
	maxcount = hms * hms;
	maxdir = 6;
	to = getdirection;
}

void location::setwalls() {
	for(auto i = 0; i < hms * hms; i++)
		game.setwall(i);
}

void gamei::initialize() {
	pathfind_initialize();
	game.setwalls();
}

void gamei::choosecards() {
	for(auto& e : bsdata<creaturei>()) {
		if(!e || !e.isplayer())
			continue;
		e.choosecards();
	}
}

void gamei::playmoves() {
	creaturea source;
	source.select();
	source.sort();
	for(auto p : source) {
		if(!(*p))
			continue;
		p->play();
	}
}

void gamei::checkinitiative() {
}

void gamei::playround() {
	choosecards();
	checkinitiative();
	playmoves();
}

void gamei::dropcoin(point hex) {
	decoration::add("Coin", hex);
}