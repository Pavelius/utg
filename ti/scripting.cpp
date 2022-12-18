#include "condition.h"
#include "main.h"

typedef void(*fnforeach)(variant v);

entitya				querry;
static playera		players;
static int			last_value;
static bool			if_able_mode;
static fnforeach	for_each;
extern entitya		onboard;

static const char* getnameshort(const char* id) {
	auto pn = getnme(str("%1Short", id));
	if(pn)
		return pn;
	return getnm(id);
}

static void choose_command_token(int count) {
	auto push_options = choosestep::options;
	choosestep::options = count;
	choosestep::run("ChooseCommandToken");
	choosestep::options = push_options;
}

static void apply_value(indicator_s v, int value) {
	if(value == 0) {
		// Simple get value
		last_value = player->get(v);
		return;
	} else if(value == 100)
		value = last_value;
	auto n0 = player->get(v);
	auto n1 = n0 + value;
	if(n1 < 0)
		return;
	switch(v) {
	case Resources:
	case Influence:
		break;
	case CommandToken:
		choose_command_token(value);
		break;
	default:
		player->indicators[v] = n1;
		//if(n1 < n0) {
		//	draw::warning(getnm("LoseIndicator"),
		//		player->getname(),
		//		getnameshort(bsdata<indicatori>::elements[v].id),
		//		iabs(n0 - n1));
		//} else {
		//	draw::information(getnm("GainIndicator"),
		//		player->getname(),
		//		getnameshort(bsdata<indicatori>::elements[v].id),
		//		iabs(n0 - n1));
		//}
		break;
	}
}

static void no_active_player(int bonus) {
	players.filter(player, false);
}

static void no_mecatol_rex(int bonus) {
	querry.filter(bsdata<systemi>::elements, false);
}

static void no_speaker(int bonus) {
	players.filter(game.speaker, false);
}

static void select_planet(int bonus) {
	querry.clear();
	querry.select(bsdata<planeti>::source);
}

static void select_planet_not_you_control(int bonus) {
	querry.clear();
	querry.select(bsdata<planeti>::source);
	querry.match(player, false);
}

static void select_planet_you_control(int bonus) {
	querry.clear();
	querry.select(bsdata<planeti>::source);
	querry.match(player, true);
}

static void select_system_own_planet(int bonus) {
	select_planet_you_control(bonus);
	querry.grouplocation(querry);
}

static void select_troop(int bonus) {
	querry.clear();
	for(auto& e : bsdata<troop>()) {
		if(!e)
			continue;
		if(e.getsystem() == systemi::active)
			querry.add(&e);
	}
}

static void select_system(int bonus) {
	querry.clear();
	querry.select(bsdata<systemi>::source);
	querry.ingame();
}

static void select_system_reach(int bonus) {
	querry.clear();
	for(auto& e : bsdata<troop>()) {
		if(!e || e.player != player || e.getunit()->type != Ships || !e.get(Move))
			continue;
		auto system = e.getsystem();
		if(system->isactivated(player))
			continue;
		querry.addreach(e.getsystem(), e.get(Move));
	}
	for(auto& e : bsdata<planeti>()) {
		if(!e || e.player != player || querry.have(e.location))
			continue;
		querry.add(e.location);
	}
}

static void select_player(int bonus) {
	players = game.players;
}

static void choose_planet(int bonus) {
	planeti::last = (planeti*)querry.choose(0);
}

static void choose_player(int bonus) {
	player = players.choose(0);
}

static void choose_system(int bonus) {
	systemi::last = (systemi*)querry.choose(0);
}

static void replenish_commodities(int bonus) {
	auto p = player;
	auto n = p->commodities - p->get(Commodities);
	apply_value(Commodities, n);
}

static void change_influence(indicator_s need, int bonus) {
	auto currency = Influence;
	auto push_options = choosestep::options;
	choosestep::options = game.rate(need, currency, bonus);
	if(choosestep::options > 0) {
		game.indicator = currency;
		choosestep::run("ChoosePay");
	}
	choosestep::options = push_options;
}

static void change_influence_command_token(int bonus) {
	change_influence(CommandToken, bonus);
}

static void activate_system(int bonus) {
	if(!systemi::last)
		return;
	game.focusing(systemi::last);
	systemi::last->setactivate(player, bonus != -1);
	systemi::active = systemi::last;
}

static void filter_system(int bonus) {
	if(!systemi::last)
		return;
	querry.match(systemi::last, bonus != -1);
}

static void filter_home_system_any(int bonus) {
	// Any home system
}

static void filter_home_system_you(int bonus) {
	// Active player home system
}

static void filter_controled(int bonus) {
	pathfind::clearpath();
	systemi::blockmove();
	systemi::markzerocost(player);
	pathfind::makewavex();
	querry.matchrange(iabs(bonus), bonus >= 0);
}

static void focus_home_system(int bonus) {
	game.focusing(player->gethome());
}

static void filter_wormhole(int bonus) {
}

static void exhaust(int bonus) {
}

static void filter_cultural_planet(int bonus) {
	querry.match(Cultural, bonus >= 0);
}

static void filter_hazardous_planet(int bonus) {
	querry.match(Hazardous, bonus >= 0);
}

static void filter_industrial_planet(int bonus) {
	querry.match(Industrial, bonus >= 0);
}

static void filter_notrait_planet(int bonus) {
	querry.match(NoTrait, bonus >= 0);
}

static void filter_production_ability(int bonus) {
	querry.match(Production, 1, bonus >= 0);
}

static void filter_red_technology(int bonus) {
	querry.match(Red, bonus >= 0);
}

static void filter_any_technology(int bonus) {
	querry.match(NoTech, bonus < 0);
}

static void filter_exhaust(int bonus) {
}

static void filter_commodities(int bonus) {
	querry.match(Commodities, bonus >= 0);
}

static void filter_activated(int bonus) {
	querry.activated(player, bonus != -1);
}

static void filter_active_player(int bonus) {
	querry.match(player, bonus != -1);
}

static void speaker(int bonus) {
	game.speaker = player;
}

static void action_card(int bonus) {
}

static void end_action(int bonus) {
	choosestep::stop = true;
}

static void redistribute_command_tokens(int bonus) {
}

static void research_technology(int bonus) {
	choosestep::run("ChooseTechnology");
}

static void score_objective(int bonus) {
}

static void if_control_mecatol_rex(int bonus) {
}

static void if_able(int bonus) {
}

static bool if_play_strategy(int bonus) {
	return player && player->use_strategy;
}

static void secret_objective(int bonus) {
}

static void querry_count(int bonus) {
	last_value = querry.getcount();
}

static void filter_move(int bonus) {
	querry.matchmove(0, bonus != -1);
}

static void action_phase_pass(int bonus) {
	player->pass_action_phase = (bonus != -1);
}

static void cancel_order(int counter) {
	onboard.clear();
}

static void move_ship(int bonus) {
	troop::last->location = systemi::active;
	for(auto p : onboard)
		p->location = systemi::active;
	game.updateui();
	choosestep::stop = true;
}

static void apply_for_each_player(variant v) {
	auto push_last = player;
	auto push_human = choosestep::human;
	for(auto p : players) {
		player = p;
		choosestep::human = player->ishuman();
		script::run(v);
	}
	choosestep::human = push_human;
	player = push_last;
}

static void for_each_player(int bonus) {
	script::apply = apply_for_each_player;
}

static void apply_for_each_troop(variant v) {
	auto push = querry;
	for(auto p : push) {
		troop::last = (troop*)p;
		script::run(v);
	}
}

static void for_each_troop(int bonus) {
	script::apply = apply_for_each_troop;
}

static void apply_for_each_planet(variant v) {
	auto push_last = planeti::last;
	for(auto p : querry) {
		planeti::last = p->getplanet();
		script::run(v);
	}
	planeti::last = push_last;
}

static void for_each_planet(int bonus) {
	script::apply = apply_for_each_planet;
}

void playeri::apply(const variants& source) {
	draw::pause();
	auto push_player = player;
	player = this;
	script::run(source);
	draw::pause();
	player = push_player;
}

template<> bool fntest<indicatori>(int index, int bonus) {
	return (player->get((indicator_s)index) + bonus) >= 0;
}
template<> void fnscript<indicatori>(int index, int bonus) {
	apply_value((indicator_s)index, bonus);
}

template<> bool fntest<playeri>(int index, int bonus) {
	return player == (bsdata<playeri>::elements + index);
}

template<> void fnscript<choosestep>(int index, int bonus) {
	auto p = bsdata<choosestep>::elements + index;
	auto push_options = choosestep::options;
	choosestep::options = bonus;
	p->run();
	choosestep::options = push_options;
}

template<> void fnscript<uniti>(int index, int bonus) {
	bsdata<uniti>::elements[index].placement(bonus);
}

void combat_reatreat(int bonus);
void combat_continue(int bonus);

BSDATA(script) = {
	{"ActionCard", action_card},
	{"ActionPhasePass", action_phase_pass},
	{"ActivateSystem", activate_system},
	{"CancelOrder", cancel_order},
	{"ChangeInfluenceCommandToken", change_influence_command_token},
	{"ChoosePlanet", choose_planet},
	{"ChoosePlayer", choose_player},
	{"ChooseSystem", choose_system},
	{"ContinueBattle", combat_continue},
	{"EndAction", end_action},
	{"Exhaust", exhaust},
	{"FilterActivated", filter_activated},
	{"FilterActivePlayer", filter_active_player},
	{"FilterAnyHomeSystem", filter_home_system_any},
	{"FilterControled", filter_controled},
	{"FilterCommodities", filter_commodities},
	{"FilterCultural", filter_cultural_planet},
	{"FilterExhaust", filter_exhaust},
	{"FilterIndustrial", filter_industrial_planet},
	{"FilterHasardous", filter_hazardous_planet},
	{"FilterHomeSystem", filter_home_system_you},
	{"FilterMoveStop", filter_move},
	{"FilterPlanetTrait", filter_notrait_planet},
	{"FilterProduction", filter_production_ability},
	{"FilterSystem", filter_system},
	{"FilterRedTechnology", filter_red_technology},
	{"FilterTechnologySpeciality", filter_any_technology},
	{"FilterWormhole", filter_wormhole},
	{"FocusHomeSystem", focus_home_system},
	{"ForEachPlanet", for_each_planet},
	{"ForEachPlayer", for_each_player},
	{"ForEachTroop", for_each_troop},
	{"IfAble", if_able},
	{"IfControlMecatolRex", if_control_mecatol_rex},
	{"IfPlayStrategy", 0, if_play_strategy},
	{"MoveShip", move_ship},
	{"NoActivePlayer", no_active_player},
	{"NoMecatolRex", no_mecatol_rex},
	{"NoSpeaker", no_speaker},
	{"QuerryCount", querry_count},
	{"RedistributeCommandTokens", redistribute_command_tokens},
	{"ReplenishCommodities", replenish_commodities},
	{"ResearchTechnology", research_technology},
	{"ScorePublicObjective", score_objective},
	{"SecretObjective", secret_objective},
	{"SelectPlanet", select_planet},
	{"SelectPlanetYouControl", select_planet_you_control},
	{"SelectPlanetNotYouControl", select_planet_not_you_control},
	{"SelectPlayer", select_player},
	{"SelectSystem", select_system},
	{"SelectSystemReach", select_system_reach},
	{"SelectSystemOwnPlanetYouControl", select_system_own_planet},
	{"SelectTroopActive", select_troop},
	{"Speaker", speaker},
	{"RetreatBattle", combat_reatreat},
};
BSDATAF(script);