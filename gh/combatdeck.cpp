#include "main.h"

void combatdeck::add(const char* id) {
	auto p = bsdata<combatcardi>::find(id);
	if(!p)
		return;
	auto count = p->count;
	if(!count)
		count = 1;
	while(count--)
		adat::add(p - bsdata<combatcardi>::elements);
}

void combatdeck::initialize() {
	clear();
	add("CombatCardB0");
	add("CombatCardB1");
	add("CombatCardP1");
	add("CombatCardB2");
	add("CombatCardP2");
	add("CombatCardX2");
	add("CombatCardMiss");
	shuffle();
}