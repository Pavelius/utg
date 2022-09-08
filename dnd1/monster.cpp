#include "main.h"

void update_monsters_stats() {
	for(auto& e : bsdata<monsteri>()) {
		e.abilities[AC] = 9 - e.abilities[AC];
	}
}

void creature::create(const struct monsteri& v) {
	clear();
	this->type = Monster;
	this->gender = Male;
	for(auto i = Strenght; i <= Charisma; i = (ability_s)(i + 1))
		basic.abilities[i] = 10;
	this->feats = v.feats;
	name = getnm(v.id);
	finish();
}