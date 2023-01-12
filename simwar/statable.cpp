#include "crt.h"
#include "statable.h"

BSDATA(costi) = {
	{"Gold", 0, 0, 4},
	{"Mana", 0, 0, 6},
	{"Happiness", 0, "%3i"},
	{"Fame"},
	{"Trade"},
	{"Lore"},
	{"Warfire", 0, "%1i/%3i"},
	{"Strenght", 0, 0, 2},
	{"Damage"},
	{"Health"},
	{"Charge"},
	{"Archery"},
	{"Level"},
	{"Armor"},
	{"Movement"},
	{"Explore"},
	{"ExploreNext"},
	{"Siege"},
	{"Size", 0, 0, 5},
	{"Limit"},
};
assert_enum(costi, Limit)

void addvalue(costa& v1, const costa& v2) {
	for(auto i = 0; i <= Limit; i++)
		v1[i] += v2[i];
}

void addvalue(costac& v1, const costac& v2) {
	for(auto i = 0; i <= Limit; i++)
		v1[i] += v2[i];
}

void subvalue(costa& v1, const costa& v2) {
	for(auto i = 0; i <= Limit; i++)
		v1[i] -= v2[i];
}

void subvalue(costac& v1, const costac& v2) {
	for(auto i = 0; i <= Limit; i++)
		v1[i] -= v2[i];
}

//bool costable::operator>=(const costable& v) const {
//	for(auto i = 0; i <= Warfire; i++) {
//		if(effect[i] < v.effect[i])
//			return false;
//	}
//	return true;
//}
//
//bool costable::operator<=(const costable& v) const {
//	for(auto i = 0; i <= Warfire; i++) {
//		if(effect[i] > v.effect[i])
//			return false;
//	}
//	return true;
//}