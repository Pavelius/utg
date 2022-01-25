#include "main.h"

static variant suffer_harm[] = {SufferHarm};
static variant exchange_harm[] = {SufferHarm, InflictHarm};

BSDATA(movei) = {
	{"EngageMelee", Combat, YouAndEnemy, {0, 1, 1}, suffer_harm, exchange_harm},
	{"GrappleEnemy", Combat, PlainMove, {0, 1, 1}},
	{"TargetSomeone", Combat, PlainMove, {0, 0, 2}},
};
assert_enum(movei, TargetSomeone)

static const char* fix_roll(result_s v) {
	switch(v) {
	case Fail: return "[-{%2i%+3i=%1i}]";
	case Success: return "{%2i%+3i=%1i}";
	default: return "[+{%2i%+3i=%1i}]";
	}
}

static void fix_roll(stringbuilder& sb) {
	sb.add(fix_roll(last_result), last_roll, last_roll_result, last_roll_bonus);
}

static const messagei* find_message(variant type, variant result) {
	for(auto& e : bsdata<messagei>()) {
		if(e.type != type || e.value || e.conditions[0] != result)
			continue;
		return &e;
	}
	return 0;
}

static int get_options_count(movemech_s v, result_s r) {
	return 0;
}

static void fix_result(stringbuilder& sb, squad* ps, move_s type, result_s result) {
	auto p = find_message(type, result);
	if(!p && result==StrongSuccess)
		p = find_message(type, Success);
	if(!p)
		return;
	ps->actv(sb, p->text, 0);
}

void squad::move(move_s action) {
	auto& ei = bsdata<movei>::elements[action];
	auto b = get(ei.ability);
	if(is(BadSpot)) {
		b -= 1;
		tags.remove(BadSpot);
	}
	if(is(Opportunity)) {
		b += 1;
		tags.remove(Opportunity);
	}
	pbta_roll(b);
	fix_roll(utg::sb);
	switch(last_roll) {
	case Fail: game.apply(ei.fail); break;
	default: game.apply(ei.success); break;
	}
	switch(ei.mechanic) {
	case YouAndEnemy:
		fix_result(utg::sb, this, action, last_result);
		break;
	default:
		break;
	}
	utg::pause();
}