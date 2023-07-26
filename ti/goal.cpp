#include "crt.h"
#include "goal.h"
#include "main.h"

static int get_non_home_planets() {
	auto result = 0;
	for(auto& e : bsdata<planeti>()) {
		if(e.player == player)
			result++;
	}
	return result;
}

static int get_specialization_planets() {
	auto result = 0;
	for(auto& e : bsdata<planeti>()) {
		if(e.player != player)
			continue;
		if(e.speciality)
			result++;
	}
	return result;
}

bool need_spend(goal_s v) {
	switch(v) {
	case ErectAMonument:
	case SwayTheCouncil:
	case NegotiateTradeRoutes:
	case LeadFromTheFront:
		return true;
	default:
		return false;
	}
}

int getgoal(goal_s v) {
	switch(v) {
	case ErectAMonument: return player->get(Resources);
	case NegotiateTradeRoutes: return player->get(TradeGoods);
	case SwayTheCouncil: return player->get(Influence);
	case LeadFromTheFront: return player->get(StrategyToken) + player->get(TacticToken);
	case ExpandBorders: return get_non_home_planets();
	case FoundResearchOutposts: return get_specialization_planets();
	default: return 0;
	}
}

BSDATA(goali) = {
	{"CornerTheMarket", 1, 4},
	{"DevelopWeaponry", 1, 2},
	{"DiversifyResearch", 1, 2},
	{"ErectAMonument", 1, 8},
	{"ExpandBorders", 1, 6},
	{"FoundResearchOutposts", 1, 3},
	{"IntimidateCouncil", 1, 2},
	{"LeadFromTheFront", 1, 3},
	{"NegotiateTradeRoutes", 1, 5},
	{"SwayTheCouncil", 1, 8},
};