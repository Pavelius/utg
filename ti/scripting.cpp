#include "condition.h"
#include "pushvalue.h"
#include "main.h"

entitya			querry;
static playera	players;
static answers	an;
static char		sb_temp[512];
static stringbuilder sb(sb_temp);
static int		last_value;
static bool		if_able_mode;
static fnevent	ai_answer_choose;
static int		choose_options;
static void*	choose_result;
static const char* choose_id;
extern entitya	onboard;
static indicator_s command_tokens[] = {TacticToken, FleetToken, StrategyToken};

static playeri* find_player(const strategyi& e) {
	for(auto p : game.players) {
		if(p->strategy == &e)
			return p;
	}
	return 0;
}

static void add_choose_options() {
	if(choose_options > 0)
		sb.adds(getnm("ChooseOptions"), choose_options);
}

static void standart_answers() {
	for(auto& e : bsdata<component>()) {
		if(strcmp(e.trigger, choose_id)!=0)
			continue;
		if(!e.isallow())
			continue;
		if(!script::allow(e.use))
			continue;
		an.add(&e, e.getname());
	}
}

static bool standart_apply() {
	if(bsdata<component>::have(choose_result))
		script::run(((component*)choose_result)->use);
	else if(bsdata<script>::have(choose_result))
		((script*)choose_result)->proc(0);
	else
		return false;
	return true;
}

static void choose_complex(const char* id, const char* cancel, fnevent add_answers, fnevent apply_answers) {
	pushvalue push_id(choose_id, id);
	draw::pause();
	sb.clear(); an.clear();
	sb.add(getnm(id));
	add_choose_options();
	add_answers();
	standart_answers();
	const char* cancel_text = 0;
	if(cancel)
		cancel_text = getnm(cancel);
	choose_result = 0;
	if(player->ishuman())
		choose_result = an.choose(sb_temp, cancel_text);
	else if(ai_answer_choose)
		ai_answer_choose();
	else
		choose_result = an.random();
	if(choose_result) {
		if(!standart_apply()) {
			if(apply_answers)
				apply_answers();
		}
	}
	draw::pause();
}

static void choose_complex(const char* id, const char* cancel, fnevent add_answers, fnevent apply_answers, int options) {
	pushvalue push(choose_options, options);
	while(choose_options > 0)
		choose_complex(id, cancel, add_answers, apply_answers);
}

static void choose_pay(const char* id, const char* cancel, fnevent add_answers, fnevent apply_answers, int options) {
	pushvalue push(choose_options, options);
	while(choose_options > 0) {
		choose_complex(id, cancel, add_answers, apply_answers);
		if(!choose_result)
			break;
	}
}

static const char* getnameshort(const char* id) {
	auto pn = getnme(str("%1Short", id));
	if(pn)
		return pn;
	return getnm(id);
}

static void ask_command_tokens() {
	if(!choose_options)
		return;
	for(auto v : command_tokens) {
		auto& e = bsdata<indicatori>::elements[v];
		an.add(&e, getnm(e.id));
	}
}
static void apply_command_tokens() {
	if(bsdata<indicatori>::have(choose_result)) {
		auto v = (indicator_s)bsdata<indicatori>::source.indexof(choose_result);
		player->add(v, 1);
		--choose_options;
	}
}
static void choose_command_token(int bonus) {
	choose_complex("ChooseCommandToken", 0, ask_command_tokens, apply_command_tokens, bonus);
}

static void ask_choose_action() {
	if(!player)
		return;
	if(player->strategy && !player->use_strategy)
		an.add(player->strategy, getnm(player->strategy->getname()));
}
static void apply_choose_action() {
	if(bsdata<strategyi>::have(choose_result)) {
		auto p = (strategyi*)choose_result;
		if(player->strategy == p) {
			script::run(p->primary);
			player->use_strategy = true;
			auto push_player = player;
			for(auto pa : game.players) {
				player = pa;
				if(script::allow(p->secondary))
					script::run(p->secondary);
			}
			player = push_player;
		} else
			script::run(p->secondary);
	}
}
static void choose_action(int bonus) {
	choose_complex("ChooseAction", 0, ask_choose_action, apply_choose_action);
}

static void ask_choose_strategy() {
	for(auto& e : bsdata<strategyi>()) {
		if(find_player(e))
			continue;
		an.add(&e, getnm(e.id));
	}
}
static void apply_choose_strategy() {
	if(bsdata<strategyi>::have(choose_result)) {
		player->strategy = (strategyi*)choose_result;
		if(player->strategy)
			player->sayspeech(player->strategy->id);
	}
}
static void choose_strategy(int bonus) {
	choose_complex("ChooseStrategy", 0, ask_choose_strategy, apply_choose_strategy);
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

static void select_planets(const playeri* player, bool iscontrol) {
	querry.clear();
	querry.select(bsdata<planeti>::source);
	querry.match(player, iscontrol);
}

static void ask_convert_resource() {
	select_planets(player, true);
	querry.match(Exhaust, false);
	querry.match(game.indicator, true);
	auto total = querry.getsummary(game.indicator);
	if(total < choose_options)
		return;
	for(auto p : querry)
		an.add(p, getnm("PayAnswer"), getnm(p->id), p->get(game.indicator));
}
static void apply_pay() {
	if(bsdata<planeti>::have(choose_result)) {
		auto p = (planeti*)choose_result;
		choose_options -= p->get(game.indicator);
		p->exhaust();
	}
}
static void convert_resource(indicator_s need, indicator_s currency, int rate) {
	pushvalue push_options(choose_options, game.rate(need, currency, rate));
	pushvalue push_currency(game.indicator, currency);
	choose_pay("ChoosePay", "DoNotPay", ask_convert_resource, apply_pay, rate);
}

static void pay_command_tokens(int bonus) {
	convert_resource(CommandToken, Influence, bonus);
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
	if(answers::interactive)
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
	//choosestep::stop = true;
}

static void redistribute_command_tokens(int bonus) {
}

static void research_technology(int bonus) {
	//choosestep::run("ChooseTechnology");
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
	lasttroop->location = systemi::active;
	for(auto p : onboard)
		p->location = systemi::active;
	game.updateui();
	//choosestep::stop = true;
}

static void apply_for_each_player(variant v) {
	auto push_last = player;
	for(auto p : players) {
		player = p;
		script::run(v);
	}
	player = push_last;
}

static void for_each_player(int bonus) {
	script::apply = apply_for_each_player;
}

static void apply_for_each_troop(variant v) {
	auto push = querry;
	for(auto p : push) {
		lasttroop = (troop*)p;
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
	{"ChooseAction", choose_action},
	{"ChooseCommandToken", choose_command_token},
	{"ChoosePlanet", choose_planet},
	{"ChoosePlayer", choose_player},
	{"ChooseStrategy", choose_strategy},
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
	{"PayCommandTokens", pay_command_tokens},
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