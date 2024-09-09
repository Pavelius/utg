#include "bsdata.h"
#include "statable.h"

BSDATA(costi) = {
	{"Resources", 0, 4},
	{"Influence", 0, 4},
	{"Gold", 0, 4},
	{"Happiness", "%3i", 10},
	{"Fame", 0, 8},
	{"Lore"},
	{"Warfire", "%1i/%3i", 1},
	{"Strenght", 0, 2},
	{"Sword"},
	{"Shield"},
	{"Death"},
	{"Movement"},
	{"Sail"},
	{"Recruit"},
	{"Size", 0, 5},
	{"Limit"},
};
assert_enum(costi, Limit)

void addvalue(costa& v1, const costa& v2) {
	for(auto i = 0; i <= Limit; i++)
		v1[i] += v2[i];
}

void subvalue(costa& v1, const costa& v2) {
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

bool isempthy(const costa& v) {
	for(auto i = 0; i <= Limit; i++) {
		if(v[i])
			return false;
	}
	return true;
}