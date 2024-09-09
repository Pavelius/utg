#include "bsdata.h"
#include "statable.h"

BSDATA(costi) = {
	{"Resources", 4},
	{"Influence", 4},
	{"Gold", 4},
	{"Happiness", 10},
	{"Fame", 8},
	{"Lore"},
	{"Units", 1},
	{"Strenght", 2},
	{"Sword"},
	{"Shield"},
	{"Death"},
	{"Movement"},
	{"Sail"},
	{"Explore"},
	{"Recruit"},
	{"Size", 5},
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