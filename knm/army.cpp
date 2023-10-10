#include "army.h"
#include "crt.h"
#include "entitya.h"
#include "pushvalue.h"
#include "variant.h"
#include "unit.h"

extern stringbuilder	console;
static const char*		last_header;
static const char*		last_list_start;
armyi					attacker, defender;
armyi*					last_army;

static void add_header(const char* id, const char* format) {
	if(equal(last_header, id))
		return;
	console.addn(format, last_header);
}

static void add_start_list() {
	last_list_start = console.get();
}

static void add_next(const char* format) {
	if(last_list_start && last_list_start[0])
		console.add(format);
}

static void add_end() {
	last_list_start = 0;
}

inline int d10() {
	return 1 + rand() % 10;
}

void armyi::clear() {
	memset(this, 0, sizeof(*this));
}

void armyi::damage(ability_s type, int chance, int count) {
	if(chance < 1)
		chance = 1;
	else if(chance > 9)
		chance = 9;
	auto need_roll = 10 - chance;
	add_start_list();
	for(auto i = 0; i < count; i++) {
		add_next(", ");
		auto result = d10();
		if(result < need_roll) {
			console.add("[~%1i]", result);
			continue;
		}
		console.add("[%1i]", result);
		add(type, 1);
	}
	add_end();
}

static void getunits(entitya& destination, entitya& source) {
	destination.clear();
	for(auto p : source)
		destination.add(p->getunit());
	destination.distinct();
}

void armyi::engage(ability_s type, int skill) {
	last_header = 0;
	for(auto p : troops) {
		add_header(p->getname(), "%1: ");
		damage(type, p->get(Combat) + skill, p->get(type));
	}
}

void armyi::resist(int& hits) {
	if(abilities[Shield] > 0) {
		if(abilities[Shield] > hits) {
			abilities[Shield] -= hits;
			hits = 0;
		} else {
			hits -= abilities[Shield];
			abilities[Shield] = 0;
		}
	}
}

void armyi::prepare(ability_s type) {
	abilities[type] = 0;
	for(auto p : troops)
		add(type, p->get(type));
}

void armyi::suffer(int hits) {
	resist(hits);
	if(hits <= 0)
		return;
	auto ps = troops.begin();
	for(auto p : troops) {
		if(hits <= 0)
			*ps++ = p;
		else
			casualty.add(p);
	}
	troops.count = ps - troops.begin();
}