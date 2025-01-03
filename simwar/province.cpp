#include "player.h"
#include "province.h"
#include "script.h"
#include "rand.h"

const short unsigned Unknown = 0xFFF0;
const short unsigned ZeroCost = 0xFFF1;
const short unsigned Blocked = 0xFFFF;

provincei* province;
provincef visibility;

static short unsigned pstart, pstop;
static short unsigned movecost[128];
static short unsigned movestack[256 * 256];

void clear_wave() {
	for(auto& e : movecost)
		e = Unknown;
}

provincei* find_zero_cost() {
	int count = bsdata<provincei>::source.getcount();
	for(int i = 0; i < count; i++) {
		if(movecost[i] == ZeroCost)
			return bsdata<provincei>::elements + i;
	}
	return 0;
}

void add_node(short unsigned index, short unsigned cost) {
	if(movecost[index] == ZeroCost) {
		if(!cost)
			movecost[index] = 0;
		else
			movecost[index] = cost - 1;
		movestack[pstop++] = index;
	} else if(movecost[index] == Unknown || (movecost[index] < Unknown && cost < movecost[index])) {
		movecost[index] = cost;
		movestack[pstop++] = index;
	}
}

void provincei::makewave() const {
	pstart = pstop = 0;
	add_node(getindex(), 0);
	while(pstart < pstop) {
		auto pi = movestack[pstart++];
		auto ci = movecost[pi];
		if(ci == Blocked)
			continue;
		ci++;
		for(auto& e : bsdata<neighbor>()) {
			if(e.n1 == pi)
				add_node(e.n2, ci);
			else if(e.n2 == pi)
				add_node(e.n1, ci);
		}
	}
}

void provincei::setzerocost() const {
	movecost[getindex()] = ZeroCost;
}

void provincei::setblocked() const {
	movecost[getindex()] = Blocked;
}

int	provincei::getcost() const {
	auto i = getindex();
	return movecost[i];
}

void neightbors::selectn(const provincei* province) {
	auto index = province->getindex();
	for(auto& e : bsdata<neighbor>()) {
		if(e.n1 == index)
			add(bsdata<provincei>::elements + e.n2);
		else if(e.n2 == index)
			add(bsdata<provincei>::elements + e.n1);
	}
}

void provincei::add(costn v, int value) {
	income[v] += value;
}

void provincei::explore(int value) {
}

bool provincei::isvisible() const {
	return visibility.is(getindex());
}