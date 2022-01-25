#include "main.h"

void statable::update_player() {
	abilities[HP] = abilities[Constitution];
}

void statable::apply_ability(int v) {
	answers an;
	for(auto i = Strenght; i <= Charisma; i = (ability_s)(i + 1)) {
		if(abilities[i])
			continue;
		an.add((void*)i, getnm(bsdata<abilityi>::get(i).id));
	}
	char temp[260]; stringbuilder sb(temp);
	sb.add(getnm("ApplyAbility"), v);
	abilities[utg::choose(an, temp, 0)] = v;
}