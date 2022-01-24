#include "main.h"

gamei		game;
int			last_count;
int			last_choose;
ability_s	last_ability;
pirate*		last_pirate;
actioni*	last_action;

bool gamei::match(variant v) {
	for(auto& e : bsdata<pirate>()) {
		if(e.match(v))
			return true;
	}
	return false;
}

static void generate_pirate() {
	auto p = bsdata<pirate>::add();
	p->clear();
	p->randomname();
}

static void generate_classes() {
	auto push_interactive = utg::interactive;
	utg::interactive = false;
	for(auto& e : bsdata<pirate>())
		e.generate();
	utg::interactive = push_interactive;
}

void gamei::clear() {
	memset(this, 0, sizeof(*this));
}

void gamei::generate() {
	bsdata<pirate>::source.clear();
	generate_pirate();
	generate_pirate();
	generate_pirate();
	generate_pirate();
	generate_classes();
	game.fillpirates();
}

void gamei::choosehistory() {
	piratea source;
	for(auto p : source)
		p->choosehistory();
}

pirate* gamei::choosepirate(const char* title, const historyable* exclude) const {
	answers an;
	for(auto& e : bsdata<pirate>()) {
		if(&e == exclude)
			continue;
		char temp[260]; temp[0] = 0;
		stringbuilder sb(temp); e.getname(sb);
		an.add(&e, temp);
	}
	return (pirate*)utg::choose(an, title);
}

static void fixerror(const char* id, ...) {
	utg::sb.addn("[-");
	utg::sb.addv(getnm(id), xva_start(id));
	utg::sb.addn("]");
}

static void special_command(special_s v, int bonus) {
	switch(v) {
	case Roll:
		if(last_pirate)
			last_pirate->roll();
		else
			fixerror("NotDefinedLastValue", "pirate");
		break;
	case Choose:
		last_choose = bonus;
		break;
	}
}

void gamei::apply(variant v) {
	switch(v.type) {
	case Ability:
		last_ability = (ability_s)v.value;
		last_count = v.counter;
		if(last_count) {
			if(last_pirate)
				last_pirate->set(last_ability, last_pirate->get(last_ability) + last_count);
			else
				fixerror("NotDefinedLastValue", "pirate");
		}
		break;
	case Special:
		special_command((special_s)v.value, v.counter);
		break;
	}
}

void gamei::apply(const variants& source) {
	for(auto v : source)
		apply(v);
}

void gamei::fillpirates() {
	auto m = bsdata<pirate>::source.getcount();
	for(unsigned i = 0; i < m; i++)
		pirates.add(i);
	zshuffle(pirates.data, m);
}

locationi& gamei::getlocation() {
	return bsdata<locationi>::elements[location];
}

pirate* gamei::getpirate(int order) const {
	if(order >= 0 && (unsigned)order < pirates.count)
		return bsdata<pirate>::elements + pirates.data[order];
	return 0;
}

void gamei::play() {
	auto push_header = utg::header;
	auto& loc = game.getlocation();
	utg::header = getnm(loc.id);
	utg::sb.clear();
	for(auto index = 0; index < sizeof(loc.actions) / sizeof(loc.actions[0]); index++) {
		if(!loc.actions[index])
			continue;
		last_action = (actioni*)loc.actions[index];
		if(!last_action)
			continue;
		piratea source(index);
		for(auto p : source) {
			p->act(utg::sb, getdescription(last_action->id));
			last_pirate = p;
			game.apply(last_action->script);
			utg::pause();
		}
	}
	utg::header = push_header;
}

void gamei::createtreasure() {
	auto m = bsdata<treasurei>::source.getcount();
	for(unsigned i = 0; i < m; i++)
		treasures.add(i);
	zshuffle(treasures.data, treasures.count);
}

const treasurei* gamei::picktreasure() {
	if(!treasures.count)
		return 0;
	auto pv = bsdata<treasurei>::elements + treasures.data[0];
	treasures.remove(0);
	return pv;
}

void gamei::getpropertyst(const void* object, variant v, stringbuilder& sb) {
	switch(v.type) {
	case Ability:
		if(last_pirate)
			pirate::getpropertyst(last_pirate, v, sb);
		break;
	}
}