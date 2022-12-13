#include "vagabond.h"

static void fix_roll(stringbuilder& sb) {
//	switch(last_result) {
//	case Fail: sb.add("[-{%2i%+3i=%1i}]", last_roll, last_roll_result, last_roll_bonus); break;
//	case Success: sb.add("{%2i%+3i=%1i}", last_roll, last_roll_result, last_roll_bonus); break;
//	default: sb.add("[+{%2i%+3i=%1i}]", last_roll, last_roll_result, last_roll_bonus); break;
//	}
}

void vagabond::roll(move_s v) {
	auto& ei = bsdata<movei>::elements[v];
}

void vagabond::move(move_s v) {
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