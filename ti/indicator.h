#include "nameable.h"
#include "stringbuilder.h"

#pragma once

enum indicator_s : unsigned char {
	TradeGoods, Commodities, Resources, Influence,
	CommandToken, FleetToken, StrategyToken, TacticToken,
	VictoryPoints,
};
struct indicatori : nameable {
	void		getinfo(stringbuilder& sb) const;
};
extern indicator_s last_indicator;
