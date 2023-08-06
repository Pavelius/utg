#include "crt.h"
#include "indicator.h"

indicator_s last_indicator;

BSDATA(indicatori) = {
	{"TradeGoods"},
	{"Commodities"},
	{"Resources"},
	{"Influence"},
	{"CommandToken"},
	{"FleetToken"},
	{"StrategyToken"},
	{"TacticToken"},
	{"VictoryPoints"},
};
assert_enum(indicatori, VictoryPoints)