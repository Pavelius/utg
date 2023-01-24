#include "crt.h"
#include "statable.h"

BSDATA(costi) = {
	{"Gold", 0, 0, 0, 4},
	{"Mana", 0, 0, 0, 6},
	{"Happiness", 0, 0, "%3i"},
	{"Fame", 0, 0, 0, 8},
	{"Faith", 0, 0, 0, 3},
	{"Lore"},
	{"Warfire", 0, 0, "%1i/%3i", 1},
	{"Strenght", 0, 0, 0, 2},
	{"Damage"},
	{"Health"},
	{"Charge"},
	{"Archery"},
	{"Level"},
	{"Armor"},
	{"Experience", 0, 0, 0, 9},
	{"Movement"},
	{"Sail"},
	{"Explore"},
	{"ExploreNext"},
	{"Siege"},
	{"Wood", FG(Resource) | FG(PerTurn)},
	{"Iron", FG(Resource) | FG(PerTurn)},
	{"Silver", FG(Resource) | FG(PerTurn)},
	{"Mithril", FG(Resource) | FG(PerTurn)},
	{"Horses", FG(Resource) | FG(PerTurn)},
	{"Mandrake", FG(Resource) | FG(PerTurn)},
	{"Recruit"},
	{"Size", 0, 0, 0, 5},
	{"Limit"},
};
assert_enum(costi, Limit)

void addvalue(costa& v1, const costa& v2) {
	for(auto i = 0; i <= Limit; i++)
		v1[i] += v2[i];
}

void addvalue(costa& v1, const costac& v2) {
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

void subvalue(costa& v1, const costac& v2) {
	for(auto i = 0; i <= Limit; i++)
		v1[i] -= v2[i];
}

bool isenought(const costa& v1, const costa& v2) {
	for(auto i = 0; i <= Limit; i++) {
		if(v2[i] && v1[i] < v2[i])
			return false;
	}
	return true;
}

bool isenought(const costa& v1, const costac& v2) {
	for(auto i = 0; i <= Limit; i++) {
		if(v1[i] < v2[i])
			return false;
	}
	return true;
}

bool isempthy(const costac& v) {
	for(auto i = 0; i <= Limit; i++) {
		if(v[i])
			return false;
	}
	return true;
}

bool isempthy(const costa& v) {
	for(auto i = 0; i <= Limit; i++) {
		if(v[i])
			return false;
	}
	return true;
}