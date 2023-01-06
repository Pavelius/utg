#include "game.h"

void get_cost_info(const void* object, stringbuilder& sb) {
	auto p = (costi*)object;
	auto v = (cost_s)(p - bsdata<costi>::elements);
	sb.addn("---");
	get_income(player, v, &sb);
}