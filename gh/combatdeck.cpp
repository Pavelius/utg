#include "main.h"

void combatdeck::add(const char* id) {
	id = szdup(id);
	for(auto& e : bsdata<combatcardi>()) {
		if(e.id == id)
			adat::add(&e - bsdata<combatcardi>::elements);
	}
}

void combatdeck::initialize() {
	clear();
	add("StandartCombatCard");
	shuffle();
}