#include "main.h"

static void captain_cabine(int bonus, int param) {
	game.script(43 + game.get(Cabine) - 1);
}

static void captain_mission(int bonus, int param) {
	game.script(48 + game.get(Mission) - 1);
}

static void check_hull(int bonus, int param) {
	auto value = game.get(Hull);
	if(value == 0)
		game.script(952);
}

static void check_crew(int bonus, int param) {
	auto value = game.get(Crew);
	auto minimum = game.get(Discontent);
	if(value <= minimum)
		game.script(951);
}

BSDATA(abilityi) = {
	{"Exploration", FG(TipsInfo) | FG(TipsLog) | FG(UseSupplyToAdd), pirate::checkexperience},
	{"Brawl", FG(TipsInfo) | FG(TipsLog) | FG(UseSupplyToAdd), pirate::checkexperience},
	{"Hunting", FG(TipsInfo) | FG(TipsLog) | FG(UseSupplyToAdd), pirate::checkexperience},
	{"Aim", FG(TipsInfo) | FG(TipsLog) | FG(UseSupplyToAdd), pirate::checkexperience},
	{"Swagger", FG(TipsInfo) | FG(TipsLog) | FG(UseSupplyToAdd), pirate::checkexperience},
	{"Navigation", FG(TipsInfo) | FG(TipsLog) | FG(UseSupplyToAdd), pirate::checkexperience},
	{"Reroll", FG(TipsInfo) | FG(TipsLog)},
	{"Misfortune", FG(TipsInfo) | FG(TipsLog) | FG(Negative)},
	{"Crew", FG(TipsInfo) | FG(TipsLog), check_crew},
	{"Discontent", FG(TipsInfo) | FG(Negative) | FG(TipsLog), check_crew},
	{"Supply", FG(TipsInfo) | FG(TipsLog)},
	{"Hull", FG(TipsInfo) | FG(TipsLog), check_hull},
	{"Danger", FG(TipsInfo) | FG(Negative) | FG(TipsLog)},
	{"DangerMaximum"},
	{"Threat", FG(Negative), gamei::threat},
	{"Mission", 0, captain_mission},
	{"MissionMaximum"},
	{"Cabine", 0, captain_cabine},
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