#include "main.h"

static void captain_cabine(const abilityi& e, int bonus) {
	game.script(43 + game.get(Cabine) - 1);
}

static void captain_mission(const abilityi& e, int bonus) {
	game.script(48 + game.get(Mission) - 1);
}

static void check_hull(const abilityi& e, int bonus) {
	auto value = game.get(Hull);
	if(value == 0)
		game.script(952);
}

static void check_crew(const abilityi& e, int bonus) {
	auto value = game.get(Crew);
	auto minimum = game.get(Discontent);
	if(value <= minimum)
		game.script(951);
}

static void check_history(const abilityi& e, int bonus) {
	game.information(getnm("YouGetHistory"));
	pausenc(getnm("GloablEvent"), game.getname());
	game.epilog(4 + game.get(History), true);
}

static void check_skill(const abilityi& e, int bonus) {
	if(bonus <= 0)
		return;
	auto a = (abilityn)(&e - bsdata<abilityi>::elements);
	auto pa = game.getclass().getearn(a);
	auto cv = game.get(a);
	for(unsigned i = 0; i < sizeof(classi::aim) / sizeof(classi::aim[0]); i++) {
		if(cv == pa[i]) {
			game.add(Stars, 1);
			break;
		}
	}
}

static void check_threat(const abilityi& e, int bonus) {
	game.stop(getnm("GloablThreat"));
	game.script(791 + game.get(Threat) - 1);
}

static void check_infamy(const abilityi& e, int bonus) {
	auto value = game.get(Infamy);
	auto maximum = game.getmaximum(Infamy);
	if(value < maximum)
		return;
	game.information(getnm("YouGainStars"));
	game.abilities[Infamy] = 0;
	if(game.abilities[Stars] < game.getmaximum(Stars))
		game.abilities[Stars]++;
}

BSDATA(abilityi) = {
	{"Exploration", FG(TipsInfo) | FG(TipsLog) | FG(UseSupplyToAdd), check_skill},
	{"Brawl", FG(TipsInfo) | FG(TipsLog) | FG(UseSupplyToAdd), check_skill},
	{"Hunting", FG(TipsInfo) | FG(TipsLog) | FG(UseSupplyToAdd), check_skill},
	{"Aim", FG(TipsInfo) | FG(TipsLog) | FG(UseSupplyToAdd), check_skill},
	{"Swagger", FG(TipsInfo) | FG(TipsLog) | FG(UseSupplyToAdd), check_skill},
	{"Navigation", FG(TipsInfo) | FG(TipsLog) | FG(UseSupplyToAdd), check_skill},
	{"Reroll", FG(TipsInfo) | FG(TipsLog)},
	{"Misfortune", FG(TipsInfo) | FG(TipsLog) | FG(Negative)},
	{"Crew", FG(TipsInfo) | FG(TipsLog), check_crew},
	{"Discontent", FG(TipsInfo) | FG(Negative) | FG(TipsLog), check_crew},
	{"Supply", FG(TipsInfo) | FG(TipsLog)},
	{"Hull", FG(TipsInfo) | FG(TipsLog), check_hull},
	{"Danger", FG(TipsInfo) | FG(Negative) | FG(TipsLog)},
	{"DangerMaximum"},
	{"Threat", FG(Negative), check_threat},
	{"Mission", 0, captain_mission},
	{"MissionMaximum"},
	{"Cabine", 0, captain_cabine},
	{"CabineMaximum"},
	{"Stars", FG(TipsInfo)},
	{"History", 0, check_history},
	{"Infamy", FG(TipsInfo) | FG(TipsLog), check_infamy},
};
assert_enum(abilityi, Infamy)

void abilityi::correct(int value, int& bonus, int min, int max) {
	value += bonus;
	if(value < min)
		bonus -= (value - min);
	else if(value > max)
		bonus -= (value - max);
}