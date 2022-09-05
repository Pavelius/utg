#include "main.h"

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

void statable::equipmentbonus(const item& it) {
	if(!it)
		return;
	auto& ei = it.geti();
	abilities[AC] += ei.armor.ac;
}