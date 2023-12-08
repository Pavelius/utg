#include "area.h"

struct arealink {
	short unsigned from, to;
};

BSDATAC(areai, 128)
BSDATAC(area, 2048)
BSDATAC(arealink, 4096)

collection<area> areas;

static arealink* find_link(short unsigned p1, short unsigned p2) {
	for(auto& e : bsdata<arealink>()) {
		if((e.from == p1 && e.to == p2) || (e.from == p2 && e.to == p1))
			return &e;
	}
	return 0;
}

void area::addlink(const area* to) {
	auto p1 = getbsi(this);
	auto p2 = getbsi(to);
	if(p1 == p2 || p1==0xFFFF || p2==0xFFFF)
		return;
	auto p = find_link(p1, p2);
	if(!p) {
		p = bsdata<arealink>::add();
		p->from = p1;
		p->to = p2;
	}
}