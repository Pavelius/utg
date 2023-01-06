#include "crt.h"
#include "costable.h"

BSDATA(costi) = {
	{"Gold"},
	{"Mana"},
	{"Happiness", 0, "%3i"},
	{"Fame"},
	{"Trade"},
	{"Lore"},
	{"Warfire", 0, "%1i/%3i"},
};
assert_enum(costi, Warfire)

void costable::operator+=(const costa& v) {
	for(auto i = 0; i <= Warfire; i++)
		cost[i] += v[i];
}

void costable::operator-=(const costa& v) {
	for(auto i = 0; i <= Warfire; i++)
		cost[i] -= v[i];
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