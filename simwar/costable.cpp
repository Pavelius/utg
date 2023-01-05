#include "crt.h"
#include "costable.h"

BSDATA(costi) = {
	{"Gold"},
	{"Food"},
	{"Wood"},
	{"Mana"},
	{"Happiness"},
	{"Fame"},
	{"Trade"},
	{"Lore"},
	{"Warfire"},
	{"Stone", 40},
	{"Iron", 40},
	{"Gems", 40},
};
assert_enum(costi, Gems)

void costable::operator+=(const costable& v) {
	for(auto i = 0; i <= Gems; i++)
		cost[i] += v.cost[i];
}

void costable::operator-=(const costable& v) {
	for(auto i = 0; i <= Gems; i++)
		cost[i] -= v.cost[i];
}

bool costable::operator>=(const costable& v) const {
	for(auto i = 0; i <= Gems; i++) {
		if(cost[i] < v.cost[i])
			return false;
	}
	return true;
}

bool costable::operator<=(const costable& v) const {
	for(auto i = 0; i <= Gems; i++) {
		if(cost[i] > v.cost[i])
			return false;
	}
	return true;
}