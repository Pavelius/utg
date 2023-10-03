#include "army.h"
#include "crt.h"
#include "entitya.h"
#include "pushvalue.h"
#include "variant.h"
#include "unit.h"

armyi	attacker, defender;

static int compare_units(const void* v1, const void* v2) {
	auto p1 = (*((entity**)v1))->getunit();
	auto p2 = (*((entity**)v2))->getunit();
	if(p1->abilities[Cost] != p2->abilities[Cost])
		return p1->abilities[Cost] - p2->abilities[Cost];
	if(p1->abilities[Army] != p2->abilities[Army])
		return p2->abilities[Army] - p2->abilities[Army];
	return p1 - p2;
}

inline int d10() {
	return 1 + rand() % 10;
}

void armyi::clear() {
	memset(this, 0, sizeof(*this));
}

void armyi::damage(ability_s type, int chance, int count) {
	for(auto i = 0; i < count; i++) {
		auto result = d10();
		if(result > chance)
			continue;
		add(type, 1);
	}
}

static void getunits(entitya& destination, entitya& source) {
	destination.clear();
	for(auto p : source)
		destination.add(p->getunit());
	destination.distinct();
}

void armyi::engage(ability_s type) {
	for(auto p : troops)
		damage(type, p->get(Combat), p->get(type));
}

void armyi::engage() {
	engage(Shoots);
	engage(Damage);
}

void armyi::sort() {
	troops.sort(compare_units);
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