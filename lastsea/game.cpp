#include "main.h"

gamei		game;
int			last_count;
ability_s	last_ability;
pirate*		last_pirate;

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
	auto push_interactive = logs::interactive;
	logs::interactive = false;
	for(auto& e : bsdata<pirate>())
		e.generate();
	logs::interactive = push_interactive;
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
	return (pirate*)logs::choose(an, title);
}

static void fixerror(const char* id, ...) {
	logs::sb.addn("[-");
	logs::sb.addv(getnm(id), xva_start(id));
	logs::sb.addn("]");
}

static void special_command(special_s v) {
	switch(v) {
	case Zero: case One: case Two: case Three: case Four: case Five:
	case Six: case Seven: case Eight: case Nine:
		last_count = v - Zero;
		break;
	case Plus:
		if(!last_count)
			fixerror("NotDefinedLastValue", "count");
		else if(last_pirate)
			last_pirate->set(last_ability, last_pirate->get(last_ability) + last_count);
		else
			fixerror("NotDefinedLastValue", "pirate");
		break;
	case Minus:
		if(!last_count)
			fixerror("NotDefinedLastValue", "count");
		else if(last_pirate)
			last_pirate->set(last_ability, last_pirate->get(last_ability) - last_count);
		else
			fixerror("NotDefinedLastValue", "pirate");
		break;
	case Roll:
		if(last_pirate)
			last_pirate->roll();
		else
			fixerror("NotDefinedLastValue", "pirate");
		break;
	}
}

void gamei::apply(variant v) {
	switch(v.type) {
	case Ability:
		last_ability = (ability_s)v.value;
		break;
	case Special:
		special_command((special_s)v.value);
		break;
	}
}

void gamei::apply(const variants& source) {
	last_count = 0;
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
	auto push_header = logs::header;
	auto& loc = game.getlocation();
	logs::header = getnm(loc.id);
	logs::sb.clear();
	for(auto index = 0; index < sizeof(loc.actions) / sizeof(loc.actions[0]); index++) {
		if(!loc.actions[index])
			continue;
		auto pa = (actioni*)loc.actions[index];
		if(!pa)
			continue;
		piratea source(index);
		for(auto p : source) {
			p->act(logs::sb, getdescription(pa->id));
			last_pirate = p;
			game.apply(pa->script);
			logs::pause();
		}
	}
	logs::header = push_header;
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