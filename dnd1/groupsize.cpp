#include "crt.h"
#include "interval.h"
#include "nameable.h"

struct groupsizei : nameable {
	interval		range;
};

BSDATA(groupsizei) = {
	{"Solitary", {1}},
	{"Few", {2, 3}},
	{"Several", {4, 7}},
	{"SmallGroup", {8, 12}},
	{"LargeGroup", {13, 16}},
	{"Lots", {17, 1000}},
};
BSDATAF(groupsizei)

const char* matchgroup(int value) {
	for(auto& e : bsdata<groupsizei>()) {
		if(e.range.have(value))
			return e.id;
	}
	return 0;
}