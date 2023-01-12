#include "building.h"
#include "province.h"
#include "script.h"
#include "unit.h"

provincei* province;

void neighbors::select(const provincei* province) {
	auto index = getbsi(province);
	clear();
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

int provincei::getunitsvalue(cost_s v) const {
	auto result = 0;
	for(auto& e : bsdata<troop>()) {
		if(e.province == this)
			result += e.type->effect[v];
	}
	return result;
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

void provincei::clearcurrent() {
	memset(current, 0, sizeof(current));
}

void provincei::update() {
	addvalue(current, income);
	addvalue(current, landscape->effect);
	buildings = getbuildings();
	units = getunits();
	strenght = getunitsvalue(Strenght);
	if(strenght)
		strenght += get(Strenght);
}