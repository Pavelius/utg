#include "main.h"

typedef void(*fnforeach)(variant v);

entitya			querry;
static playera		players;
static int			last_value;
static bool			if_able_mode;
static fnforeach	for_each;

static void no_active_player(int bonus, int param) {
	players.filter(game.active, false);
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
	case 1: querry.match(playeri::last, true); break;
	case 2: querry.match(playeri::last, false); break;
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

static void activate_system(int bonus, int param) {
	if(!systemi::last)
		return;
	systemi::last->setactivate(playeri::last, bonus != -1);
}

static void filter_system(int bonus, int param) {
	if(!systemi::last)
		return;
	querry.match(systemi::last, bonus != -1);
}

static void filter_home_system(int bonus, int param) {
	// param = 0 Active player home system
	// param = 1 Any home system
}

static void filter_wormhole(int bonus, int param) {
}

static void exhaust(int bonus, int param) {
}

static void filter_planet_type(int bonus, int param) {
	querry.match((planet_trait_s)param, bonus != -1);
}

static void filter_technology_speciality(int bonus, int param) {
	querry.match((color_s)param, bonus != -1);
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

static void if_able(int bonus, int param) {
}

static void secret_objective(int bonus, int param) {
}

static void querry_count(int bonus, int param) {
	last_value = querry.getcount();
}

static void set_for_each(int bonus, int param) {
	for_each = (fnforeach)param;
}

static void apply_value(indicator_s v, int value) {
	if(value == 0) {
		// Simple get value
		last_value = playeri::last->get(v);
		return;
	} else if(value == 100)
		value = last_value;
	auto n0 = playeri::last->get(v);
	auto n1 = n0 + value;
	if(n1 < 0)
		return;
	switch(v) {
	case Resources:
	case Influence:
		break;
	case CommandToken:
		playeri::last->addcommand(value);
		break;
	default:
		playeri::last->indicators[v] = n1;
		break;
	}
}

static void apply_value(variant v) {
	if(v.iskind<scripti>()) {
		auto p = bsdata<scripti>::elements + v.value;
		p->proc(v.counter, p->param);
	} else if(v.iskind<indicatori>())
		apply_value((indicator_s)v.value, v.counter);
	else
		draw::warning(getnm("ErrorScriptType"), bsdata<varianti>::get(v.type).id);
}

static void for_each_player(variant v) {
	auto push_last = playeri::last;
	for(auto p : players) {
		playeri::last = p;
		apply_value(v);
	}
	playeri::last = push_last;
}

static void for_each_planet(variant v) {
	auto push_last = planeti::last;
	for(auto p : querry) {
		planeti::last = p->getplanet();
		apply_value(v);
	}
	planeti::last = push_last;
}

static void apply_foreach(variant v) {
	if(for_each)
		for_each(v);
	else
		apply_value(v);
	for_each = 0;
}

void playeri::apply(const variants& source) {
	draw::pause();
	last = game.active;
	for_each = 0;
	for(auto v : source)
		apply_foreach(v);
	draw::pause();
}

BSDATA(scripti) = {
	{"ActionCard", action_card},
	{"ActivateSystem", activate_system},
	{"BuyCommandToken", buy_command_token},
	{"ChoosePlanet", choose_planet},
	{"ChoosePlayer", choose_player},
	{"ChooseSystem", choose_system},
	{"Exhaust", exhaust},
	{"FilterActivated", filter_activated},
	{"FilterAnyHomeSystem", filter_home_system, 1},
	{"FilterCultural", filter_planet_type, Cultural},
	{"FilterExhaust", filter_exhaust},
	{"FilterIndustrial", filter_planet_type, Industrial},
	{"FilterHasardous", filter_planet_type, Hazardous},
	{"FilterHomeSystem", filter_home_system},
	{"FilterPlanetTrait", filter_planet_type, NoTrait},
	{"FilterSystem", filter_system},
	{"FilterTechnologySpeciality", filter_technology_speciality},
	{"FilterWormhole", filter_wormhole},
	{"ForEachPlanet", set_for_each, (int)for_each_planet},
	{"ForEachPlayer", set_for_each, (int)for_each_player},
	{"IfAble", if_able},
	{"IfControlMecatolRex", if_control_mecatol_rex},
	{"NextAction", next_action},
	{"NoActivePlayer", no_active_player},
	{"NoMecatolRex", no_mecatol_rex},
	{"NoSpeaker", no_speaker},
	{"PDSorDock", pds_or_dock, 0},
	{"QuerryCount", querry_count},
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