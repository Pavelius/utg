#include "main.h"

gamei game;

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

void gamei::generate() {
	bsdata<pirate>::source.clear();
	generate_pirate();
	generate_pirate();
	generate_pirate();
	generate_pirate();
	generate_classes();
}

void gamei::choosehistory() {
	for(auto& e : bsdata<pirate>())
		e.choosehistory();
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