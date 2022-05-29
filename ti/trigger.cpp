#include "main.h"

BSDATA(triggeri) = {
	{"AsAction"},
	{"StartRoundSpaceCombat"},
	{"EndRoundGroundCombat"},
	{"EndStrategyPhase"},
};
assert_enum(triggeri, EndStrategyPhase)