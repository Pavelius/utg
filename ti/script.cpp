#include "main.h"

static orderablea	querry;
static playera		players;

static void no_active_player(int bonus, int param) {
	players.filter(playeri::active, false);
}

static void no_mecatol_rex(int bonus, int param) {
	querry.filter(bsdata<systemi>::elements, false);
}

static void no_speaker(int bonus, int param) {
	players.filter(game.speaker, false);
}

static void select_planet(int bonus, int param) {
	querry.clear();
	querry.select(bsdata<planeti>::source);
	switch(param) {
	case 1: querry.match(playeri::active, true); break;
	case 2: querry.match(playeri::active, false); break;
	default: break;
	}
}

static void select_system_own_planet(int bonus, int param) {
	select_planet(bonus, param);
	querry.selectlocation(querry);
}

static void select_system(int bonus, int param) {
	querry.clear();
	querry.select(bsdata<systemi>::source);
}

static void select_player(int bonus, int param) {
	players = game.players;
}

static void choose_planet(int bonus, int param) {
	planeti::last = (planeti*)querry.choose(0);
}

static void choose_player(int bonus, int param) {
	playeri::last = players.choose(0);
}

static void choose_system(int bonus, int param) {
	systemi::last = (systemi*)querry.choose(0);
}

static void replenish_commodities(int bonus, int param) {
	auto p = playeri::last;
	p->set(Commodities, p->commodities);
}

static void buy_command_token(int bonus, int param) {
	auto p = playeri::last;
}

static void each_player_activate_system(int bonus, int param) {
	if(!systemi::last)
		return;
	for(auto player : players)
		systemi::last->setactivate(player, bonus != -1);
}

static void activate_system(int bonus, int param) {
	if(!systemi::last)
		return;
	systemi::last->setactivate(playeri::active, bonus != -1);
}

static void filter_system(int bonus, int param) {
	if(!systemi::last)
		return;
	querry.match(systemi::last, bonus != -1);
}

static void exhaust(int bonus, int param) {
}

static void filter_exhaust(int bonus, int param) {
}

static void filter_activated(int bonus, int param) {
}

static void speaker(int bonus, int param) {
	game.speaker = playeri::last;
}

static void action_card(int bonus, int param) {
}

static void next_action(int bonus, int param) {
}

static void pds_or_dock(int bonus, int param) {
}

static void redistribute_command_tokens(int bonus, int param) {
}

static void research_technology(int bonus, int param) {
}

static void score_objective(int bonus, int param) {
}

static void if_control_mecatol_rex(int bonus, int param) {
}

static void secret_objective(int bonus, int param) {
}

BSDATA(scripti) = {
	{"ActionCard", action_card},
	{"ActivateSystem", activate_system},
	{"BuyCommandToken", buy_command_token},
	{"ChoosePlanet", choose_planet},
	{"ChoosePlayer", choose_player},
	{"ChooseSystem", choose_system},
	{"EachPlayerActivateSystem", each_player_activate_system},
	{"Exhaust", exhaust},
	{"FilterActivated", filter_activated},
	{"FilterExhaust", filter_exhaust},
	{"FilterSystem", filter_system},
	{"IfControlMecatolRex", if_control_mecatol_rex},
	{"NextAction", next_action},
	{"NoActivePlayer", no_active_player},
	{"NoMecatolRex", no_mecatol_rex},
	{"NoSpeaker", no_speaker},
	{"PDSorDock", pds_or_dock, 0},
	{"RedistributeCommandTokens", redistribute_command_tokens},
	{"ReplenishCommodities", replenish_commodities},
	{"ResearchTechnology", research_technology},
	{"ScorePublicObjective", score_objective},
	{"SecretObjective", secret_objective},
	{"SelectPlanet", select_planet, 0},
	{"SelectPlanetYouControl", select_planet, 1},
	{"SelectPlanetNotYouControl", select_planet, 2},
	{"SelectPlayer", select_player},
	{"SelectSystem", select_system, 1},
	{"SelectSystemOwnPlanetYouControl", select_system_own_planet, 1},
	{"Speaker", speaker},
};
BSDATAF(scripti);