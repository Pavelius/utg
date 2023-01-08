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

//void costable::operator+=(const costa& v) {
//	for(auto i = 0; i <= Warfire; i++)
//		effect[i] += v[i];
//}
//
//void costable::operator-=(const costa& v) {
//	for(auto i = 0; i <= Warfire; i++)
//		effect[i] -= v[i];
//}
//
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