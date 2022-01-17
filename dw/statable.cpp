#include "main.h"

int statable::last_roll;
result_s statable::last_result;

result_s statable::rollv(int b) {
	if(b < -3)
		b = -3;
	else if(b > 3)
		b = 3;
	last_roll = 2 + rand() % 6 + rand() % 6 + b;
	if(last_roll <= 6)
		last_result = Miss;
	else if(last_roll >= 12)
		last_result = CriticalSuccess;
	else if(last_roll >= 10)
		last_result = Success;
	else
		last_result = PartialSuccess;
	return last_result;
}

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
	abilities[logs::choose(an, temp, 0)] = v;
}