#include "draw_utg.h"
#include "generator.h"
#include "treasure.h"

struct treasurei {
	struct coini {
		char	percent;
		dice	range;
		short	multiplier;
	};
	char	symbol;
	coini	cp, sp, ep, gp, pp, gems, jewelry, magic;
};

BSDATA(treasurei) = {
	{'A', {25, {1, 6}, 1000}, {30, {1, 6}, 1000}, {20, {1, 4}, 1000}, {35, {2, 6}, 1000}, {25, {1, 2}, 1000}, {50, {6, 6}}, {50, {6, 6}}, {30, {3}}},
	{'B', {50, {1, 8}, 1000}, {25, {1, 6}, 1000}, {25, {1, 4}, 1000}, {25, {1, 3}, 1000}, {}, {25, {1, 6}}, {25, {1, 6}}, {10, {1}}},
	{'C', {20, {1, 12}, 1000}, {30, {1, 4}, 1000}, {10, {1, 4}, 1000}, {}, {}, {25, {1, 4}}, {25, {1, 4}}, {10, {2}}},
	{'D', {10, {1, 8}, 1000}, {15, {1, 12}, 1000}, {}, {60, {1, 6}, 1000}, {}, {30, {1, 8}}, {30, {1, 8}}, {15, {2}}},
	{'E', {5, {1, 10}, 1000}, {30, {1, 12}, 1000}, {25, {1, 4}, 1000}, {25, {1, 8}, 1000}, {}, {10, {1, 10}}, {10, {1, 10}}, {25, {3}}},
	{'F', {}, {10, {1, 12}, 1000}, {25, {1, 4}, 1000}, {25, {1, 8}, 1000}, {}, {10, {1, 10}}, {10, {1, 10}}, {25, {3}}},
};
BSDATAF(treasurei)

const treasurei* find_treasure(char symbol) {
	for(auto& e : bsdata<treasurei>()) {
		if(e.symbol == symbol)
			return &e;
	}
	return 0;
}

static void add_coins(treasure& result, const char* id, const treasurei::coini& e) {
	auto pi = bsdata<itemi>::find(id);
	if(!pi)
		return;
	if(e.percent < 100) {
		if(d100() >= e.percent)
			return;
	}
	auto count = e.range.roll();
	if(e.multiplier)
		count = count * e.multiplier;
	item it; it.create(pi);
	it.setcount(count);
	result.add(it);
}

static void add_gems(treasure& result, const treasurei::coini& e) {
	if(e.percent < 100) {
		if(d100() >= e.percent)
			return;
	}
	auto count = e.range.roll();
	if(e.multiplier)
		count = count * e.multiplier;
	for(auto i = 0; i < count; i++) {
		auto value = random_value("RandomGems");
		if(value.iskind<itemi>()) {
			item it; it.create(value.value, 1);
			result.add(it);
		}
	}
}

static void add_jewelry(treasure& result, const treasurei::coini& e) {
	if(e.percent < 100) {
		if(d100() >= e.percent)
			return;
	}
	auto count = e.range.roll();
	if(e.multiplier)
		count = count * e.multiplier;
	variant jewelry = "Jewelry";
	if(!jewelry)
		return;
	for(auto i = 0; i < count; i++) {
		auto value = random_list_value(jewelry);
		if(value.iskind<itemi>()) {
			item it; it.create(value.value, 1);
			result.add(it);
		}
	}
}

void treasure::add(item it) {
	for(auto& e : *this)
		e.add(it);
	if(it)
		adat<item>::add(it);
}

void treasure::generate(char symbol) {
	auto ps = find_treasure(symbol);
	if(!ps)
		return;
	add_coins(*this, "CP", ps->cp);
	add_coins(*this, "SP", ps->sp);
	add_coins(*this, "EP", ps->ep);
	add_coins(*this, "GP", ps->gp);
	add_coins(*this, "PP", ps->pp);
	add_gems(*this, ps->gems);
	add_jewelry(*this, ps->jewelry);
}

void treasure::take() {
	char temp[260]; stringbuilder sb(temp);
	while(!draw::isnext()) {
		an.clear();
		for(auto& e : *this) {
			sb.clear(); e.addname(sb);
			an.add(&e, temp);
		}
		if(!an)
			break;
		auto p = (item*)an.choose(getnm("WhatItemToTake"), getnm("LeaveItHere"));
		if(!p)
			break;
	}
}