#include "draw_utg.h"
#include "item.h"
#include "rand.h"
#include "randomizer.h"
#include "roll.h"

struct treasurei {
	struct coini {
		char		percent;
		interval	range;
	};
	char	symbol;
	coini	cp, sp, ep, gp, pp, gems, jewelry, magic;
};

BSDATA(treasurei) = {
	// Lair
	{'A', {25, {1000, 6000}}, {30, {1000, 6000}}, {20, {1000, 4000}}, {35, {2000, 12000}}, {25, {1000, 2000}}, {50, {6, 36}}, {50, {6, 36}}, {30, {3}}},
	{'B', {50, {1000, 8000}}, {25, {1000, 6000}}, {25, {1000, 4000}}, {25, {1000, 3000}}, {}, {25, {1, 6}}, {25, {1, 6}}, {10, {1}}},
	{'C', {20, {1000, 12000}}, {30, {1000, 4000}}, {10, {1000, 4000}}, {}, {}, {25, {1, 4}}, {25, {1, 4}}, {10, {2}}},
	{'D', {10, {1000, 8000}}, {15, {1000, 12000}}, {}, {60, {1000, 6000}}, {}, {30, {1, 8}}, {30, {1, 8}}, {15, {2}}},
	{'E', {5, {1000, 10000}}, {30, {1000, 12000}}, {25, {1000, 4000}}, {25, {1000, 8000}}, {}, {10, {1, 10}}, {10, {1, 10}}, {25, {3}}},
	{'F', {}, {10, {1000, 12000}}, {25, {1000, 4000}}, {25, {1000, 8000}}, {}, {10, {1, 10}}, {10, {1, 10}}, {25, {3}}},
	// Individual
	{'P', {100, {3, 24}}},
	{'Q', {}, {100, {3, 18}}},
	{'R', {100, {2, 12}}},
	{'S', {}, {}, {}, {100, {2, 8}}},
	{'T', {}, {}, {}, {}, {100, {1, 6}}},
	// Group
	{'U', {10, {1, 100}}, {5, {1, 100}}, {}, {10, {1, 100}}, {}, {5, {1, 4}}, {5, {1, 4}}, {2, {1}}},
	{'V', {}, {10, {1, 100}}, {5, {1, 100}}, {10, {1, 100}}, {5, {1, 100}}, {10, {1, 4}}, {10, {1, 4}}, {5, {1}}},
};
BSDATAF(treasurei)

const treasurei* find_treasure(char symbol) {
	for(auto& e : bsdata<treasurei>()) {
		if(e.symbol == symbol)
			return &e;
	}
	return 0;
}

static void add_coins(const char* id, const treasurei::coini& e) {
	auto pi = bsdata<itemi>::find(id);
	if(!pi)
		return;
	if(e.percent < 100) {
		if(d100() >= e.percent)
			return;
	}
	auto count = e.range.roll();
	item it(pi);
	it.setcount(count);
	it.drop();
}

static void add_gems(const treasurei::coini& e) {
	if(e.percent < 100) {
		if(d100() >= e.percent)
			return;
	}
	auto count = e.range.roll();
	for(auto i = 0; i < count; i++) {
		auto value = single("RandomGems");
		if(value.iskind<itemi>()) {
			item it(bsdata<itemi>::elements + value.value);
			it.drop();
		}
	}
}

static void add_jewelry(const treasurei::coini& e) {
	if(e.percent < 100) {
		if(d100() >= e.percent)
			return;
	}
	auto count = e.range.roll();
	for(auto i = 0; i < count; i++) {
		auto value = single("Jewelry");
		if(value.iskind<itemi>()) {
			item it(bsdata<itemi>::elements + value.value);
			it.drop();
		}
	}
}

static void generate_one(const treasurei* ps) {
	add_coins("CP", ps->cp);
	add_coins("SP", ps->sp);
	add_coins("EP", ps->ep);
	add_coins("GP", ps->gp);
	add_coins("PP", ps->pp);
	add_gems(ps->gems);
	add_jewelry(ps->jewelry);
}

static void generate_one(char symbol) {
	auto ps = find_treasure(symbol);
	if(ps)
		generate_one(ps);
}

void generate_lair_treasure(const char* symbols) {
	while(*symbols) {
		if(*symbols < 'P')
			generate_one(symbols[0]);
		symbols++;
	}
}

void generate_treasure(const char* symbols, int group_count) {
	while(*symbols) {
		if(symbols[0] >= 'P' && symbols[0] <= 'T') {
			for(auto i = 0; i < group_count; i++)
				generate_one(symbols[0]);
		} else
			generate_one(symbols[0]);
		symbols++;
	}
}

//void treasure::take() {
//	char temp[260]; stringbuilder sb(temp);
//	while(!draw::isnext()) {
//		an.clear();
//		for(auto& e : *this) {
//			sb.clear(); e.addname(sb);
//			an.add(&e, temp);
//		}
//		if(!an)
//			break;
//		auto p = (item*)an.choose(getnm("WhatItemToTake"), getnm("LeaveItHere"));
//		if(!p)
//			break;
//	}
//}