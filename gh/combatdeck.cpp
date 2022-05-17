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

typedef int(*fncompare)(const void* v1, const void* v2);

static combatcardi* takeone(combatdeck& original, int count, fncompare proc) {
	if(count <= 1)
		return 0;
	adat<combatcardi*> source;
	for(auto i = 0; i < count; i++)
		source.add(original.take());
	qsort(source.data, source.count, sizeof(source.data[0]), proc);
	auto result = source.data[0];
	for(size_t i = 1; i < source.count; i++)
		original.discard(source.data[i]);
	return result;
}

static int compare_good(const void* v1, const void* v2) {
	auto p1 = *((combatcardi**)v1);
	auto p2 = *((combatcardi**)v2);
	return p1->bonus - p2->bonus;
}

static int compare_bad(const void* v1, const void* v2) {
	auto p1 = *((combatcardi**)v1);
	auto p2 = *((combatcardi**)v2);
	return p2->bonus - p1->bonus;
}

combatcardi* combatdeck::takegood(int count) {
	return takeone(*this, count, compare_good);
}

combatcardi* combatdeck::takebad(int count) {
	return takeone(*this, count, compare_good);
}