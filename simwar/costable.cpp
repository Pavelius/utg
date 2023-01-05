#include "crt.h"
#include "costable.h"

BSDATA(costi) = {
	{"Gold"},
	{"Mana"},
	{"Happiness"},
	{"Fame"},
	{"Trade"},
	{"Lore"},
	{"Warfire"},
};
assert_enum(costi, Warfire)

void costable::operator+=(const costable& v) {
	for(auto i = 0; i <= Warfire; i++)
		cost[i] += v.cost[i];
}

void costable::operator-=(const costable& v) {
	for(auto i = 0; i <= Warfire; i++)
		cost[i] -= v.cost[i];
}

bool costable::operator>=(const costable& v) const {
	for(auto i = 0; i <= Warfire; i++) {
		if(cost[i] < v.cost[i])
			return false;
	}
	return true;
}

bool costable::operator<=(const costable& v) const {
	for(auto i = 0; i <= Warfire; i++) {
		if(cost[i] > v.cost[i])
			return false;
	}
	return true;
}