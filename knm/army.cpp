#include "army.h"
#include "crt.h"
#include "entitya.h"
#include "player.h"
#include "pushvalue.h"
#include "variant.h"
#include "unit.h"

extern stringbuilder	console;
static const char*		last_header;
static const char*		last_list_start;
armyi					attacker, defender;
armyi					*last_army, *winner_army;

static void add_header() {
	if(!last_header)
		return;
	if(!last_army)
		return;
	console.addn(last_header, last_army->player->getname());
	last_header = 0;
}

static void add_next(const char* format) {
	if(last_list_start && last_list_start[0])
		console.add(format);
}

inline int d10() {
	return 1 + rand() % 10;
}

void armyi::setheader(const char* format) {
	last_header = format;
}

void armyi::applycasualty() {
	for(auto p : casualty)
		p->clear();
}

void armyi::clear() {
	memset(this, 0, sizeof(*this));
}

int	armyi::getsummary(ability_s v) const {
	return troops.gettotal(v) + get(v);
}

void armyi::damage(int chance, int count) {
	if(chance < 1)
		chance = 1;
	else if(chance > 10)
		chance = 10;
	auto need_roll = 11 - chance;
	for(auto i = 0; i < count; i++) {
		add_next(", ");
		auto result = d10();
		if(result < need_roll) {
			console.add("%1i", result);
			continue;
		}
		console.add("[%1i]", result);
		add(Damage, 1);
	}
}

void armyi::engage(const char* name, int chance, int count) {
	if(count <= 0)
		return;
	add_header();
	console.addn("%1: ", name);
	last_list_start = console.get();
	damage(chance, count);
}

void armyi::engage(ability_s type, int skill) {
	entitya querry = troops;
	querry.group(get_unit);
	for(auto p : querry)
		engage(p->getname(), p->get(Combat) + skill, troops.gettotal(p) * p->get(type));
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
		else {
			casualty.add(p);
			hits--;
		}
	}
	troops.count = ps - troops.begin();
}

void armyi::select(const entity* location, playeri* player) {
	this->player = player;
	troops.select(location, player);
	troops.sortunits();
}