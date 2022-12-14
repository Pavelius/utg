#include "pushvalue.h"
#include "result.h"
#include "quest.h"
#include "vagabond.h"

vagabond* player;

static void fix_roll(stringbuilder& sb) {
	switch(last_result) {
	case Fail: sb.add("[-{%2i%+3i=%1i}]", last_roll, last_roll_result, last_roll_bonus); break;
	case Success: sb.add("{%2i%+3i=%1i}", last_roll, last_roll_result, last_roll_bonus); break;
	default: sb.add("[+{%2i%+3i=%1i}]", last_roll, last_roll_result, last_roll_bonus); break;
	}
}

void vagabond::move(move_s v) {
	auto push_move = lastmove; lastmove = bsdata<movei>::elements + v;
	inflict.clear();
	suffer.clear();
	auto bonus = player->get(lastmove->roll);
	if(player->forward.is(v))
		bonus++;
	player->forward.remove(v);
	bonus += player->forward_any; player->forward_any = 0;
	pbta_roll(bonus);
	fix_roll(console);
	lastmove->run();
	lastmove = push_move;
}

//void creature::apply(const effectable& effect) {
//	static harm_s harms[] = {Injury, Exhaustion, Wear, Morale, Depletion};
//	auto ni = effect.inflict.getdistinct();
//	auto ns = effect.suffer.getdistinct();
//	if(ni == 0 && ns == 0)
//		return;
//	actv(utg::sb, getname(), 0);
//	if(ni) {
//		actv(utg::sb, "%-Inflict", 0);
//		effect.inflict.getinfo(utg::sb, harms);
//	}
//	if(ns) {
//		if(ni)
//			utg::sb.add(", %-But");
//		actv(utg::sb, "%-Suffer", 0);
//		effect.suffer.getinfo(utg::sb, harms);
//		for(auto v : harms)
//			add(v, effect.suffer.harm[v]);
//	}
//	utg::sb.add(".");
//}