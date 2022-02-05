#include "main.h"

void chest::gaintreasure(treasurei* p) {
	if(p)
		p->setowner(this);
}

void chest::losstreasure(treasurei* p) {
	if(p)
		p->setowner(0);
}

int chest::getbonus(ability_s v) const {
	auto r = 0;
	for(auto& e : bsdata<treasurei>()) {
		if(!e.isactive() || e.isdiscarded())
			continue;
		r += e.abilities[v - Exploration];
	}
	return r;
}

int chest::gettreasurecount(tag_s v) const {
	auto r = 0;
	for(auto& e : bsdata<treasurei>()) {
		if(!e.isactive() || e.isdiscarded() || !e.is(Valuable))
			continue;
		r++;
	}
	return r;
}

treasurei* chest::choosetreasure(const char* title, const char* cancel) const {
	answers an;
	for(auto& e : bsdata<treasurei>()) {
		if(!e.isactive() || e.isdiscarded() || !e.is(Valuable))
			continue;
		an.add(&e, getnm(e.id));
	}
	return (treasurei*)an.choose(title, cancel);
}

void chest::apply(trigger_s type, ability_s v) {
	for(auto& e : bsdata<treasurei>()) {
		if(!e.isactive() || e.isdiscarded())
			continue;
		if(e.trigger != type)
			continue;
		if(e.ability != v)
			continue;
		e.apply();
	}
}