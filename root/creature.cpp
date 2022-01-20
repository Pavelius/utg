#include "main.h"

effectable game;

class apply_move : public logs::choosei {
	void apply(int index, const void* object) override {
		auto p = (messagei*)object;
		for(auto v : p->conditions) {
			if(v)
				game.apply(v);
		}
	}
public:
	apply_move(answers& an) : choosei(an) {}
};

static void choose_options(variant v, int choose_count) {
	if(!choose_count)
		return;
	answers an;
	for(auto& e : bsdata<messagei>()) {
		if(e.type != v)
			continue;
		if(!e.value)
			continue;
		an.add(&e, e.text);
	}
	apply_move source(an);
	source.choose(0, choose_count);
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
	case Fail: sb.add("[-{%2i%+3i=%1i}]", last_roll, last_roll_result, last_roll_bonus); break;
	case Success: sb.add("{%2i%+3i=%1i}", last_roll, last_roll_result, last_roll_bonus); break;
	default: sb.add("[+{%2i%+3i=%1i}]", last_roll, last_roll_result, last_roll_bonus); break;
	}
}

const messagei* find_message(variant type, variant result) {
	for(auto& e : bsdata<messagei>()) {
		if(e.value != 0 || e.type != type)
			continue;
		if(e.conditions[0] == result)
			return &e;
	}
	return 0;
}

static void fix_move(stringbuilder& sb, move_s v, const creature* pc) {
	auto p = find_message(v, last_result);
	if(!p)
		return;
	sb.addsep(' ');
	pc->acts(sb, p->text);
}

void creature::move(move_s v) {
	auto& ei = bsdata<movei>::elements[v];
	pbta_roll(get(ei.roll));
	fix_roll(logs::sb);
	fix_move(logs::sb, v, this);
	choose_options(v, get_choose_count(ei.mechanic));
}

void rangeable::addrange(int v) {
	int i = range + v;
	if(i < Intimate)
		i = Intimate;
	else if(i > Far)
		i = Far;
	range = (tag_s)i;
}

void creature::apply(const effectable& effect) {
	static harm_s harms[] = {Injury, Exhaustion, Wear, Morale, Depletion};
	auto ni = effect.inflict.getdistinct();
	auto ns = effect.suffer.getdistinct();
	if(ni == 0 && ns == 0)
		return;
	act(getname());
	if(ni) {
		act("%-Inflict");
		effect.inflict.fix(logs::sb, harms);
	}
	if(ns) {
		if(ni)
			logs::sb.add(", %-But");
		act("%-Suffer");
		effect.suffer.fix(logs::sb, harms);
		for(auto v : harms)
			add(v, effect.suffer.harm[v]);
	}
	logs::sb.add(".");
}