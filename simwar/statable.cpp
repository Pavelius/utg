#include "crt.h"
#include "statable.h"

BSDATA(costi) = {
	{"Gold"},
	{"Mana"},
	{"Happiness", 0, "%3i"},
	{"Fame"},
	{"Trade"},
	{"Lore"},
	{"Warfire", 0, "%1i/%3i"},
	{"Strenght"},
	{"Damage"},
	{"Health"},
	{"Charge"},
	{"Archery"},
	{"Level"},
	{"Armor"},
	{"Movement"},
	{"Explore"},
	{"Siege"},
	{"Size"},
	{"Limit"},
};
assert_enum(costi, Limit)

void addvalue(costa& v1, const costa& v2) {
	for(auto i = 0; i <= Warfire; i++)
		v1[i] += v2[i];
}

void subvalue(costa& v1, const costa& v2) {
	for(auto i = 0; i <= Warfire; i++)
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