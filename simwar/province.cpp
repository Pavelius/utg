#include "building.h"
#include "province.h"
#include "script.h"
#include "unit.h"

provincei* province;

void provincei::getneighbors(neighbort& result) const {
	auto index = getbsi(this);
	result.clear();
	for(auto& e : bsdata<neighbor>()) {
		if(e.n1 == index)
			result.add(bsdata<provincei>::elements + e.n2);
		else if(e.n2 == index)
			result.add(bsdata<provincei>::elements + e.n1);
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