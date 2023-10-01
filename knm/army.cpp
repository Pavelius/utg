#include "army.h"
#include "crt.h"
#include "pushvalue.h"
#include "variant.h"
#include "unit.h"

armyi attacker, defender;

inline int d10() {
	return rand() % 10;
}

static void apply_action(armyi& enemy, variant v);

static void apply_action(armyi& enemy, const variants& source) {
	for(auto v : source)
		apply_action(enemy, v);
}

static void apply_action(armyi& enemy, variant v) {
	if(v.iskind<abilityi>())
		enemy.damage((ability_s)v.value, v.counter);
}

void armyi::clear() {
	memset(this, 0, sizeof(*this));
}

void armyi::damage(ability_s type, int chance) {
	if(chance && d10() >= chance)
		return;
	add(type, 1);
}

void armyi::engage(armyi& enemy) {
	for(auto p : units)
		apply_action(*this, p->getunit()->combat);
}