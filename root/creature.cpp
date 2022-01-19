#include "main.h"

effectable game;

static void apply_effect(void* object) {
	auto p = (messagei*)object;
	for(auto v : p->conditions)
		game.apply(v);
}

static void make_move(variant v, int choose_count) {
	answers an;
	for(auto& e : bsdata<messagei>()) {
		if(!e.match(v))
			continue;
		if(!e.value)
			continue;
		an.add(&e, e.text);
	}
	logs::apply(an, 0, apply_effect, choose_count);
}

void creature::roll(move_s v) {
	auto& ei = bsdata<movei>::elements[v];
}

static int get_choose_count(move_mechanic_s v) {
	static int result[Choose2or3 + 1][CriticalSuccess + 1] = {
		{},
		{0, 1, 1, 1},
		{0, 1, 2, 2},
		{0, 1, 3, 3},
		{0, 2, 3, 3},
	};
	return result[v][last_result];
}

static void fix_roll(stringbuilder& sb) {
	switch(last_result) {
	case Fail: sb.add("[-{%1i}]", last_roll); break;
	case PartialSuccess: sb.add("{%1i}", last_roll); break;
	default: sb.add("[+{%1i}]", last_roll); break;
	}
}

const messagei* find_message(variant type, variant result) {
	for(auto& e : bsdata<messagei>()) {
		if(e.value != 0)
			continue;
		if(e.conditions[0]==type && e.conditions[1] == result)
			return &e;
	}
	return 0;
}

static void fix_move(stringbuilder& sb, move_s v, const creature* pc) {
	auto p = find_message(v, last_result);
	if(!p)
		return;
	sb.addsep(' ');
	pc->act(sb, p->text);
}

void creature::move(move_s v) {
	auto& ei = bsdata<movei>::elements[v];
	pbaroll(get(ei.roll));
	fix_roll(logs::sb);
	fix_move(logs::sb, v, this);
	make_move(v, get_choose_count(ei.mechanic));
}

void rangeable::addrange(int v) {
	int i = range + v;
	if(i < Intimate)
		i = Intimate;
	else if(i > Far)
		i = Far;
	range = (tag_s)i;
}