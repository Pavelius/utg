#include "main.h"

BSDATA(abilityi) = {
	{"Exploration", FG(TipsInfo) | FG(TipsLog) | FG(UseSupplyToAdd)},
	{"Brawl", FG(TipsInfo) | FG(TipsLog) | FG(UseSupplyToAdd)},
	{"Hunting", FG(TipsInfo) | FG(TipsLog) | FG(UseSupplyToAdd)},
	{"Aim", FG(TipsInfo) | FG(TipsLog) | FG(UseSupplyToAdd)},
	{"Swagger", FG(TipsInfo) | FG(TipsLog) | FG(UseSupplyToAdd)},
	{"Navigation", FG(TipsInfo) | FG(TipsLog) | FG(UseSupplyToAdd)},
	{"Reroll", FG(TipsInfo) | FG(TipsLog)},
	{"Misfortune", FG(TipsInfo) | FG(TipsLog) | FG(Negative)},
	{"Crew", FG(TipsInfo) | FG(TipsLog)},
	{"Discontent", FG(TipsInfo) | FG(Negative) | FG(TipsLog)},
	{"Supply", FG(TipsInfo) | FG(TipsLog)},
	{"Hull", FG(TipsInfo) | FG(TipsLog), 952},
	{"Danger", FG(TipsInfo) | FG(Negative) | FG(TipsLog)},
	{"DangerMaximum"},
	{"Threat", FG(Negative)},
	{"Mission"},
	{"MissionMaximum"},
	{"Cabine"},
	{"CabineMaximum"},
	{"Stars", FG(TipsInfo)},
	{"History"},
	{"Infamy", FG(TipsInfo) | FG(TipsLog)},
};
assert_enum(abilityi, Infamy)

void abilityi::correct(int value, int& bonus, int min, int max) {
	value += bonus;
	if(value < min)
		bonus -= (value - min);
	else if(value > max)
		bonus -= (value - max);
}