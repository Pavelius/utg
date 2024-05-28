#include "nameable.h"
#include "stringbuilder.h"

#pragma once

enum indicator_s : unsigned char {
	TradeGoods, Commodities, Resources, Influence,
	CommandToken, FleetToken, StrategyToken, TacticToken,
	VictoryPoints,
};
struct indicatori : nameable {
};
extern indicator_s last_indicator;
