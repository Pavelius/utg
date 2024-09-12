#include "moveorder.h"

BSDATAC(moveorder, 64)

static moveorder* find(short unsigned from, short unsigned to) {
	for(auto& e : bsdata<moveorder>()) {
		if(e.from == from && e.to == to)
			return &e;
	}
	return 0;
}

void add_move(const provincei* from, const provincei* to, playeri* player, int count) {
	auto p1 = getbsi(from);
	auto p2 = getbsi(to);
	auto p = find(p1, p2);
	if(!p) {
		p = bsdata<moveorder>::add();
		p->from = p1;
		p->to = p2;
		p->player = player;
		p->count = 0;
	}
	count += p->count;
	if(count < 0)
		count = 0;
	p->count = count;
}