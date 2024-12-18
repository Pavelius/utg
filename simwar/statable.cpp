#include "bsdata.h"
#include "statable.h"

BSDATA(costi) = {
	{"Resources", 20},
	{"Influence", 23},
	{"Gold", 22},
	{"Happiness", 10},
	{"Fame", 8},
	{"Lore", 14},
	{"Warfire", 1},
	{"Strenght", 18},
	{"Sword", 19},
	{"Shield", 2},
	{"Death", 17},
	{"Movement"},
	{"Sail"},
	{"Recruit"},
	{"Size", 5},
	{"War"},
	{"Trade"},
	{"Culture"},
	{"Noble"},
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

costn pay_alternate(costn v) {
	switch(v) {
	case Resources: return Gold;
	case Influence: return Gold;
	default: return v;
	}
}