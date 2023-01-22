#include "building.h"
#include "hero.h"
#include "player.h"
#include "province.h"
#include "script.h"
#include "unit.h"

const short unsigned Unknown = 0xFFF0;
const short unsigned ZeroCost = 0xFFF1;
const short unsigned Blocked = 0xFFFF;

provincei*	province;
provincef	visibility;

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

int provincei::getbuildings() const {
	auto count = 0;
	for(auto& e : bsdata<building>()) {
		if(e.province == this)
			count++;
	}
	return count;
}

int provincei::getsites() const {
	auto count = 0;
	for(auto& e : bsdata<site>()) {
		if(e.province == this)
			count++;
	}
	return count;
}

int provincei::getunits() const {
	auto count = 0;
	for(auto& e : bsdata<troop>()) {
		if(e.province == this)
			count++;
	}
	return count;
}

void provincei::add(cost_s v, int value) {
	income[v] += value;
	switch(v) {
	case Explore: case ExploreNext:
		if(income[v] >= 100)
			income[v] = 100;
		break;
	}
}

void provincei::explore(int value) {
	if(!income[ExploreNext])
		income[ExploreNext] = xrand(10, 30);
	add(Explore, value);
	while(income[Explore] >= income[ExploreNext] && income[ExploreNext] < 100) {
		auto push_province = province; province = this;
		script::run("RandomSite", 0);
		script::run("AddSite", 0);
		province = push_province;
		add(ExploreNext, xrand(8, 20));
	}
}

void provincei::update() {
	memcpy(current, income, sizeof(current));
	addvalue(current, landscape->effect);
	subvalue(current, landscape->upkeep);
	buildings = getbuildings();
	units = getunits();
	defend = current[Strenght];
	attack = 0;
}

bool provincei::isvisible() const {
	return visibility.is(getindex());
}