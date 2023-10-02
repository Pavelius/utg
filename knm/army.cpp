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

void armyi::damage(ability_s type, int chance, int count, int chance_critical) {
	for(auto i = 0; i < count; i++) {
		auto result = d10();
		if(result > chance)
			continue;
		if(d10() <= chance_critical)
			add(CriticalDamage, 1);
		else
			add(type, 1);
	}
}

static void getunits(entitya& destination, entitya& source) {
	destination.clear();
	for(auto p : source)
		destination.add(p->getunit());
	destination.distinct();
}

int	armyi::getcount(entity* type) const {
	auto result = 0;
	for(auto p : troops) {
		if(p->getunit() == type)
			result++;
	}
	return result;
}

void armyi::engage(armyi& enemy, ability_s attack_type, ability_s damage_type) {
	entitya units;
	getunits(units, troops);
	for(auto p : units) {
		auto count = getcount(p);
		if(!count)
			continue;
		enemy.damage(damage_type, p->get(Combat), p->get(attack_type) * count, 1);
	}
}

void armyi::engage(armyi& enemy) {
	engage(enemy, LightDamage, LightDamage);
	engage(enemy, HeavyDamage, HeavyDamage);
}

void armyi::sort() {
	troops.sort(compare_units);
}