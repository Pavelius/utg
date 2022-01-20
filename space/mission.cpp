#include "main.h"

const int maximum_danger = 5;

missioni mission;

void missioni::clear() {
	memset(this, 0, sizeof(*this));
}

void missioni::add(const squad& v) {
}

void missioni::add(const char* id) {
	auto i = bsdata<varianti>::elements[Squad].found(id);
	if(i == -1)
		return;
	squad e = {};
	add(e);
}

bool missioni::dangerous() {
	if(danger < maximum_danger) {
		danger++;
		return false;
	} else {
		danger = 0;
		return true;
	}
}

bool missioni::lucky() {
	if(luck < maximum_danger) {
		luck++;
		return false;
	} else {
		luck = 0;
		return true;
	}
}