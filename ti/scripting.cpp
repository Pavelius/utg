#include "condition.h"
#include "main.h"

typedef void(*fnforeach)(variant v);

entitya				querry;
static playera		players;
static int			last_value;
static bool			if_able_mode;
static fnforeach	for_each;
extern entitya		onboard;

static void choose_command_token(int count) {
	auto push_options = choosestep::options;
	choosestep::options = count;
	choosestep::run("ChooseCommandToken");
	choosestep::options = push_options;
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
		choose_command_token(value);
		break;
	default:
		playeri::last->indicators[v] = n1;
		if(n1 < n0) {
			draw::warning(getnm("LoseIndicator"),
				playeri::last->getname(),
				getnm(bsdata<indicatori>::elements[v].id),
				iabs(n0 - n1));
		} else {
			draw::information(getnm("GainIndicator"),
				playeri::last->getname(),
				getnm(bsdata<indicatori>::elements[v].id),
				iabs(n0 - n1));
		}
		break;
	}
}

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
	querry.grouplocation(querry);
}

static void select_troop(int bonus, int param) {
	querry.clear();
	for(auto& e : bsdata<troop>()) {
		if(!e)
			continue;
		if(e.getsystem() == systemi::active)
			querry.add(&e);
	}
}

static void select_system(int bonus, int param) {
	querry.clear();
	querry.select(bsdata<systemi>::source);
	querry.ingame();
}

static void select_system_reach(int bonus, int param) {
	auto player = game.active;
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
	auto n = p->commodities - p->get(Commodities);
	apply_value(Commodities, n);
}

static void change_influence(int bonus, int param) {
	auto need = (indicator_s)param;
	auto currency = Influence;
	auto push_options = choosestep::options;
	choosestep::options = game.rate(need, currency, bonus);
	if(choosestep::options > 0) {
		game.indicator = currency;
		choosestep::run("ChoosePay");
	}
	choosestep::options = push_options;
}

static void activate_system(int bonus, int param) {
	if(!systemi::last)
		return;
	game.focusing(systemi::last);
	systemi::last->setactivate(playeri::last, bonus != -1);
	systemi::active = systemi::last;
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

static void filter_controled(int bonus, int param) {
	pathfind::clearpath();
	systemi::blockmove();
	systemi::markzerocost(game.active);
	pathfind::makewavex();
	querry.matchrange(iabs(bonus), bonus >= 0);
}

static void focus_home_system(int bonus, int param) {
	game.focusing(game.active->gethome());
}

static void filter_wormhole(int bonus, int param) {
}

static void exhaust(int bonus, int param) {
}

static void filter_planet_type(int bonus, int param) {
	querry.match((planet_trait_s)param, bonus != -1);
}

static void filter_unit_ability(int bonus, int param) {
	querry.match((ability_s)param, 1, bonus != -1);
}

static void filter_technology_speciality(int bonus, int param) {
	querry.match((color_s)param, bonus != -1);
}

static void filter_exhaust(int bonus, int param) {
}

static void filter_indicator(int bonus, int param) {
	querry.match((indicator_s)param, bonus != -1);
}

static void filter_activated(int bonus, int param) {
	querry.activated(playeri::last, bonus != -1);
}

static void filter_active_player(int bonus, int param) {
	querry.match(game.active, bonus != -1);
}

static void speaker(int bonus, int param) {
	game.speaker = playeri::last;
}

static void action_card(int bonus, int param) {
}

static void end_action(int bonus, int param) {
	choosestep::stop = true;
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

static void filter_move(int bonus, int param) {
	querry.matchmove(param, bonus != -1);
}

static void action_phase_pass(int bonus, int param) {
	playeri::last->pass_action_phase = (bonus != -1);
}

static void cancel_order(int counter, int param) {
	onboard.clear();
}

static void move_ship(int bonus, int param) {
	troop::last->location = systemi::active;
	for(auto p : onboard)
		p->location = systemi::active;
	game.updateui();
	choosestep::stop = true;
}

static void for_each_player(variant v) {
	auto push_last = playeri::last;
	auto push_human = choosestep::human;
	for(auto p : players) {
		playeri::last = p;
		choosestep::human = playeri::last->ishuman();
		script::run(v);
	}
	choosestep::human = push_human;
	playeri::last = push_last;
}

static void for_each_player_active(variant v) {
	auto push_last = playeri::last;
	auto push_human = choosestep::human;
	for(auto p : players) {
		game.active = p;
		playeri::last = p;
		choosestep::human = playeri::last->ishuman();
		script::run(v);
	}
	choosestep::human = push_human;
	playeri::last = push_last;
}

static void for_each_troop(variant v) {
	auto push = querry;
	for(auto p : push) {
		troop::last = (troop*)p;
		script::run(v);
	}
}

static void for_each_planet(variant v) {
	auto push_last = planeti::last;
	for(auto p : querry) {
		planeti::last = p->getplanet();
		script::run(v);
	}
	planeti::last = push_last;
}

static void script_run(variant v) {
	if(v.iskind<indicatori>())
		apply_value((indicator_s)v.value, v.counter);
	else if(v.iskind<playeri>()) {
		if(game.active != bsdata<playeri>::elements + v.value)
			script::stop = true;
	} else if(v.iskind<choosestep>()) {
		auto p = bsdata<choosestep>::elements + v.value;
		auto push_options = choosestep::options;
		choosestep::options = v.counter;
		p->run();
		choosestep::options = push_options;
	} else if(v.iskind<uniti>())
		bsdata<uniti>::elements[v.value].placement(v.counter);
	else {
		auto& ei = bsdata<varianti>::get(v.type);
		draw::warning(getnm("ErrorScriptType"), ei.id);
	}
}

void playeri::apply(const variants& source) {
	draw::pause();
	last = game.active;
	script::run(source);
	draw::pause();
}

static bool script_test(variant v, bool& allowed) {
	if(v.iskind<playeri>())
		allowed = (game.active == (bsdata<playeri>::elements + v.value));
	else if(v.iskind<indicatori>()) {
		auto i = (indicator_s)v.value;
		auto b = v.counter;
		allowed = (game.active->get(i) + b) >= 0;
	} else
		return false;
	return true;
}

static bool if_play_strategy(int counter, int param) {
	return game.active && game.active->use_strategy;
}

static void condition_initialize() {
	conditioni::add("IfPlayStrategy", if_play_strategy);
}

void script_initialize() {
	script::prun = script_run;
	script::ptest = script_test;
	condition_initialize();
}

BSDATA(script) = {
	{"ActionCard", action_card},
	{"ActionPhasePass", action_phase_pass},
	{"ActivateSystem", activate_system},
	{"CancelOrder", cancel_order},
	{"ChangeInfluenceCommandToken", change_influence, CommandToken},
	{"ChoosePlanet", choose_planet},
	{"ChoosePlayer", choose_player},
	{"ChooseSystem", choose_system},
	{"EndAction", end_action},
	{"Exhaust", exhaust},
	{"FilterActivated", filter_activated},
	{"FilterActivePlayer", filter_active_player},
	{"FilterAnyHomeSystem", filter_home_system, 1},
	{"FilterControled", filter_controled},
	{"FilterCommodities", filter_indicator, Commodities},
	{"FilterCultural", filter_planet_type, Cultural},
	{"FilterExhaust", filter_exhaust},
	{"FilterIndustrial", filter_planet_type, Industrial},
	{"FilterHasardous", filter_planet_type, Hazardous},
	{"FilterHomeSystem", filter_home_system},
	{"FilterMoveStop", filter_move, 0},
	{"FilterPlanetTrait", filter_planet_type, NoTrait},
	{"FilterProduction", filter_unit_ability, Production},
	{"FilterSystem", filter_system},
	{"FilterTechnologySpeciality", filter_technology_speciality},
	{"FilterWormhole", filter_wormhole},
	{"FocusHomeSystem", focus_home_system},
	{"ForEachPlanet", script::setforeach, (int)for_each_planet},
	{"ForEachPlayer", script::setforeach, (int)for_each_player},
	{"ForEachPlayerActive", script::setforeach, (int)for_each_player_active},
	{"ForEachTroop", script::setforeach, (int)for_each_troop},
	{"IfAble", if_able},
	{"IfControlMecatolRex", if_control_mecatol_rex},
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
	{"SelectPlanet", select_planet, 0},
	{"SelectPlanetYouControl", select_planet, 1},
	{"SelectPlanetNotYouControl", select_planet, 2},
	{"SelectPlayer", select_player},
	{"SelectSystem", select_system, 1},
	{"SelectSystemReach", select_system_reach},
	{"SelectSystemOwnPlanetYouControl", select_system_own_planet, 1},
	{"SelectTroopActive", select_troop},
	{"Speaker", speaker},
};
BSDATAF(script);