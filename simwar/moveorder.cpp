#include "moveorder.h"
#include "player.h"
#include "province.h"

BSDATAC(moveorder, 64)

void moveorder::clear() {
	memset(this, 0, sizeof(*this));
}

static moveorder* find(short unsigned from, short unsigned to) {
	for(auto& e : bsdata<moveorder>()) {
		if(e.from == from && e.to == to)
			return &e;
	}
	return 0;
}

provincei* moveorder::getfrom() const {
	return bsdata<provincei>::elements + from;
}

provincei* moveorder::getto() const {
	return bsdata<provincei>::elements + to;
}

void add_move(const provincei* from, const provincei* to, playeri* player, int count) {
	auto p1 = getbsi(from);
	auto p2 = getbsi(to);
	auto p = find(p1, p2);
	if(!p) {
		p = bsdata<moveorder>::addz();
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

void cancel_move(const provincei* to, const playeri* player) {
	auto p2 = getbsi(to);
	for(auto& e : bsdata<moveorder>()) {
		if(e.to == p2 && e.player == player) {
			e.getfrom()->units += e.count;
			e.clear();
		}
	}
}