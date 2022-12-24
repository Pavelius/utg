#include "condition.h"
#include "pushvalue.h"
#include "main.h"

entitya			querry, onboard;
static playera	players;
static answers	an;
static char		sb_temp[512];
static stringbuilder sb(sb_temp);
static int		last_value;
static bool		if_able_mode;
static int		choose_options;
static bool		choose_stop;
static void*	choose_result;
static const char* choose_id;
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

static void addscript(const char* id) {
	auto p = bsdata<script>::find(id);
	if(p)
		an.add(p, getnm(p->id));
}

static void makewave(pathfind::indext start) {
	pathfind::clearpath();
	systemi::blockmove();
	systemi::blockenemy(player);
	pathfind::makewave(start);
	systemi::blockenemy(player);
}

void entitya::addreach(const systemi* system, int range) {
	pathfind::clearpath();
	systemi::blockmove();
	pathfind::makewave(system->index);
	for(auto i = 0; i < pathfind::maxcount; i++) {
		auto move = pathfind::getmove(i);
		if(move == pathfind::Blocked || move > range)
			continue;
		auto ps = systemi::findbyindex(i);
		if(ps && !have(ps))
			add(ps);
	}
}

static int ai_load(const playeri* player, const systemi* system, unit_type_s type, int capacity) {
	if(capacity <= 0)
		return 0;
	entitya ships;
	ships.select(player, system, type);
	ships.matchload(true);
	auto result = 0;
	while(result < capacity && ships) {
		auto p1 = ships[0];
		p1->location = systemi::active;
		ships.remove(0);
		result++;
	}
	return result;
}

static void standart_answers() {
	for(auto& e : bsdata<component>()) {
		if(strcmp(e.trigger, choose_id) != 0)
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

static int d100() {
	return rand() % 100;
}

static void choose_complex(const char* id, const char* cancel, fnevent add_answers, fnevent apply_answers, fnevent ai_answers) {
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
		choose_result = an.choose(sb_temp, cancel_text, 2);
	else if(ai_answers)
		ai_answers();
	else {
		if(cancel && d100() < 10)
			choose_result = 0;
		else
			choose_result = an.random();
	}
	if(choose_result) {
		if(!standart_apply()) {
			if(apply_answers)
				apply_answers();
		}
	}
	draw::pause();
}

static void choose_complex(const char* id, const char* cancel, fnevent add_answers, fnevent apply_answers, fnevent ai_answers, int options) {
	pushvalue push(choose_options, options);
	while(choose_options > 0)
		choose_complex(id, cancel, add_answers, apply_answers, ai_answers);
}

static void choose_pay(const char* id, const char* cancel, fnevent add_answers, fnevent apply_answers, fnevent ai_answers, int options) {
	pushvalue push(choose_options, options);
	while(choose_options > 0) {
		choose_complex(id, cancel, add_answers, apply_answers, ai_answers);
		if(!choose_result)
			break;
	}
}

static void choose_until_stop(const char* id, const char* cancel, fnevent add_answers, fnevent apply_answers, fnevent ai_answers) {
	pushvalue push(choose_stop, false);
	while(!choose_stop) {
		choose_complex(id, cancel, add_answers, apply_answers, ai_answers);
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
	choose_complex("ChooseCommandToken", 0, ask_command_tokens, apply_command_tokens, 0, bonus);
}

static void apply_secondanary_ability(strategyi& e) {
	if(!script::allow(e.secondary))
		return;
	if(player->ishuman()) {
		if(!draw::yesno(getnm("ApplySecondanaryStrategy"), e.getname()))
			return;
		script::run(e.secondary);
	} else {
		if(d100() < 50)
			script::run(e.secondary);
	}
}

static void apply_primary_ability(strategyi& e) {
	player->use_strategy = true;
	script::run(e.primary);
	auto push_player = player;
	for(auto pa : game.players) {
		if(pa == push_player)
			continue;
		player = pa;
		apply_secondanary_ability(e);
	}
	player = push_player;
}

static void ask_choose_action() {
	if(player->strategy && !player->use_strategy)
		an.add(player->strategy, getnm(player->strategy->getname()));
}
static void apply_choose_action() {
	if(bsdata<strategyi>::have(choose_result)) {
		auto p = (strategyi*)choose_result;
		if(player->strategy == p)
			apply_primary_ability(*p);
	}
}
static void choose_action(int bonus) {
	choose_complex("ChooseAction", 0, ask_choose_action, apply_choose_action, 0);
}

static void ask_movement() {
	makewave(systemi::active->index);
	for(auto& e : bsdata<troop>()) {
		if(e.player != player)
			continue;
		auto pu = e.getunit();
		if(!pu)
			continue;
		auto move = e.get(Move);
		if(!move)
			continue;
		auto ps = e.getsystem();
		if(!ps || ps->isactivated(player))
			continue;
		auto cost_move = pathfind::getmove(ps->index);
		if(cost_move == pathfind::Blocked || move < cost_move)
			continue;
		an.add(&e, "%1 (%2)", e.getname(), e.location->getsystem()->getname());
	}
}
static void apply_movement() {
	if(bsdata<troop>::have(choose_result)) {
		pushvalue push(lasttroop, (troop*)choose_result);
		script::run("ChooseMoveOption", 0);
	}
}
static void ai_movement() {
	entitya ships;
	ships.select(an);
	for(auto p : ships) {
		auto capacity = p->get(Capacity);
		auto player = p->player;
		auto system = p->getsystem();
		capacity -= ai_load(player, system, Ships, capacity);
		capacity -= ai_load(player, system, GroundForces, capacity);
		p->location = systemi::active;
	}
	game.updateui();
}
static void choose_movement(int bonus) {
	onboard.clear();
	choose_until_stop("ChooseMove", "EndMovement", ask_movement, apply_movement, ai_movement);
	game.updatecontrol();
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
	choose_complex("ChooseStrategy", 0, ask_choose_strategy, apply_choose_strategy, 0);
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
	planeti::last = (planeti*)querry.choose(0, 0, 2);
	game.focusing(planeti::last->getsystem());
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
	choose_pay("ChoosePay", "DoNotPay", ask_convert_resource, apply_pay, 0, rate);
}
static void complex_pay(indicator_s need, int rate) {
	pushvalue push_options(choose_options, rate);
	pushvalue push_currency(game.indicator, need);
	choose_pay("PayResources", 0, ask_convert_resource, apply_pay, 0, rate);
}

static void pay_command_tokens(int bonus) {
	convert_resource(CommandToken, Influence, bonus);
}

static void ask_invasion() {
	for(auto& e : bsdata<troop>()) {
		if(e.player != player)
			continue;
		auto pu = e.getunit();
		if(!pu || pu->type != GroundForces)
			continue;
		auto move = e.get(Move);
		if(move > 0)
			continue;
		auto ps = e.getsystem();
		if(ps != systemi::active)
			continue;
		if(e.location == planeti::last) {
			if(player->ishuman())
				an.add(&e, "%1 (%Cancel)", e.getname());
		} else if(!bsdata<planeti>::have(e.location))
			an.add(&e, e.getname());
	}
}
static void apply_invasion() {
	if(bsdata<troop>::have(choose_result)) {
		auto p = (troop*)choose_result;
		if(p->location == planeti::last)
			p->location = planeti::last->getsystem();
		else
			p->location = planeti::last;
	}
}
static void ai_invasion() {
	auto system = systemi::active;
	// Select all planets in a system
	entitya planets;
	planets.selectplanets(system);
	if(!planets)
		return;
	// Select all ground units in a space
	entitya ground;
	ground.select(player, system);
	ground.match(GroundForces, true);
	if(!ground)
		return;
	auto average = ground.getcount() / planets.getcount();
	if(average < 1)
		average = 1;
	for(auto planet : planets) {
		auto count = 0;
		for(auto p : ground) {
			if(count >= average)
				break;
			if(p->location != system)
				continue;
			p->location = planet;
			count++;
		}
	}
	game.updateui();
}
static void choose_invasion(int bonus) {
	choose_until_stop("ChooseInvasion", "Apply", ask_invasion, apply_invasion, ai_invasion);
}

static bool have_invasion_units() {
	auto push = an;
	ask_invasion();
	auto result = an.getcount() != 0;
	an = push;
	return result;
}

static void ask_invasion_planet() {
	if(!have_invasion_units())
		return;
	for(auto& e : bsdata<planeti>()) {
		if(e.location != systemi::active)
			continue;
		an.add(&e, e.getname());
	}
}
static void apply_invasion_planet() {
	if(bsdata<planeti>::have(choose_result)) {
		auto push_last = planeti::last;
		auto push_result = choose_result;
		planeti::last = (planeti*)choose_result;
		script::run("ChooseInvasion", 0);
		choose_result = push_result;
		planeti::last = push_last;
		game.updateui();
		game.updatecontrol();
	}
}
static void choose_invasion_planet(int bonus) {
	choose_until_stop("ChooseInvasionPlanet", "EndInvasion", ask_invasion_planet, apply_invasion_planet, 0);
}

static void ask_move_options() {
	auto ship = lasttroop;
	auto system = ship->getsystem();
	auto capacity = ship->get(Capacity);
	sb.clear();
	sb.add(getnm("ChooseMoveOption"), ship->getname(), systemi::active->getname());
	char temp[260]; stringbuilder sbt(temp);
	addscript("MoveShip");
	if(capacity > 0) {
		for(auto& e : bsdata<troop>()) {
			if(e.player != player || e.getsystem() != system)
				continue;
			if(e.getunit()->type == Structures)
				continue;
			if(e.get(Move) != 0)
				continue;
			sbt.clear();
			if(onboard.have(&e)) {
				if(player->ishuman())
					sbt.add(getnm("UnboardTroop"), e.getname());
			} else if(onboard.getcount() < capacity)
				sbt.add(getnm("BoardTroop"), e.getname());
			else
				continue;
			if(e.location && e.location != system)
				sbt.adds("(%1)", e.location->getname());
			an.add(&e, temp);
		}
	}
}
static void apply_onboard() {
	if(bsdata<troop>::have(choose_result)) {
		auto p = (troop*)choose_result;
		auto index = onboard.find(p);
		if(index == -1)
			onboard.add(p);
		else
			onboard.remove(index, 1);
	}
}
static void choose_move_options(int bonus) {
	choose_until_stop("ChooseMoveOptions", "CancelMovement", ask_move_options, apply_onboard, 0);
	onboard.clear();
}

static void ask_production() {
	if(!lasttroop)
		return;
	auto production = lasttroop->getproduction();
	if(!production)
		return;
	auto player = lasttroop->player;
	auto resources = player->getplanetsummary(Resources);
	auto goods = player->get(TradeGoods);
	auto production_count = onboard.getsummary(CostCount);
	auto total_production = production - production_count;
	auto total_resources = goods + resources - onboard.getsummary(Cost);
	if(onboard.getcount() > 0) {
		sb.addn("---");
		for(auto p : onboard) {
			auto pu = (uniti*)p;
			sb.addn("%1 - %Cost [%2i]", p->getname(), pu->getcost());
			if(pu->abilities[CostCount] > 1)
				sb.adds("%Count [%1i]", pu->abilities[CostCount]);
		}
	}
	sb.addn("---");
	sb.addn(getnm("ProductionTotal"), total_resources, total_production);
	for(auto& e : bsdata<prototype>::elements[player->getindex()].units) {
		if(e.type == Structures)
			continue;
		auto cost = e.getcost();
		if(cost > total_resources || total_production <= 0)
			continue;
		auto total_count = player->getsummary(&e);
		if(total_count >= e.abilities[Reinforcement])
			continue;
		an.add(&e, "%1 (%Cost %2i)", e.getname(), e.getcost());
	}
	if(player->ishuman()) {
		if(onboard)
			addscript("CancelOrder");
	}
}
static void apply_production() {
	if(bsdata<uniti>::have(choose_result) || bsdata<prototype>::have(choose_result))
		onboard.add((entity*)choose_result);
}
static void choose_production(int bonus) {
	auto pu = lasttroop;
	onboard.clear();
	choose_until_stop("ChooseProduction", "EndBuild", ask_production, apply_production, 0);
	complex_pay(Resources, onboard.getsummary(Cost));
	for(auto p : onboard)
		pu->produce((uniti*)p);
	game.updateui();
}

static void ask_dock() {
	for(auto& e : bsdata<troop>()) {
		if(e.player != player || !e.get(Production) || e.getsystem() != systemi::active)
			continue;
		e.add(an);
	}
}
static void apply_dock() {
	if(bsdata<troop>::have(choose_result))
		lasttroop = (troop*)choose_result;
}
static void choose_dock(int bonus) {
	choose_complex("ChooseDock", 0, ask_dock, apply_dock, 0);
}

static bool allow(const uniti* pu) {
	auto maximum_count = pu->abilities[MaximumInOneLocation];
	if(maximum_count > 0) {
		if(pu->type == Structures) {
			entitya source;
			source.select(player, planeti::last);
			if(source.getsummary(pu) >= maximum_count)
				return false;
		}
	}
	return true;
}
static void add_unit(answers& an, const char* id) {
	auto pu = bsdata<uniti>::find(id);
	if(!pu || !allow(pu))
		return;
	an.add(pu, getnm(pu->id));
}
static void ask_pds_or_dock() {
	add_unit(an, "PDS");
	add_unit(an, "SpaceDock");
}
static void apply_pds_or_dock() {
	if(bsdata<uniti>::have(choose_result))
		((uniti*)choose_result)->placement(1);
}
static void choose_pds_or_dock(int bonus) {
	choose_complex("ChoosePDSorDock", 0, ask_pds_or_dock, apply_pds_or_dock, 0);
}

static void ask_technology() {
	requirement advance = {};
	player->getadvance(advance);
	for(auto& e : bsdata<techi>()) {
		auto i = getbsi(&e);
		if(player->tech.is(i))
			continue;
		if(!e.match(advance))
			continue;
		an.add(&e, getnm(e.id));
	}
}
static void apply_technology() {
	if(bsdata<techi>::have(choose_result))
		player->tech.set(getbsi((techi*)choose_result));
}
static void choose_technology(int bonus) {
	choose_complex("ChooseTechnology", 0, ask_technology, apply_technology, 0);
}

static void ask_combat_option() {
	sb.add("%1 (%-Round [%2i])", getnm("ChooseCombatOption"), army::round);
	addscript("ContinueBattle");
	if(player->ishuman())
		addscript("RetreatBattle");
}
static void choose_combat_option(int bonus) {
	choose_complex("ChooseCombatOption", 0, ask_combat_option, 0, 0);
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

static void redistribute_command_tokens(int bonus) {
}

static void research_technology(int bonus) {
	choose_technology(0);
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

static void end_turn(int bonus) {
	choose_stop = true;
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
	choose_stop = true;
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
	auto push_player = player;
	auto push_header = answers::header;
	player = this;
	answers::header = player->getname();
	script::run(source);
	answers::header = push_header;
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
	if(player)
		bsdata<prototype>::elements[player->getindex()].units[index].placement(bonus);
	else
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
	{"ChooseCombatOption", choose_combat_option},
	{"ChooseCommandToken", choose_command_token},
	{"ChooseDock", choose_dock},
	{"ChooseInvasion", choose_invasion},
	{"ChooseInvasionPlanet", choose_invasion_planet},
	{"ChooseMove", choose_movement},
	{"ChooseMoveOption", choose_move_options},
	{"ChoosePDSorDock", choose_pds_or_dock},
	{"ChoosePlanet", choose_planet},
	{"ChoosePlayer", choose_player},
	{"ChooseProduction", choose_production},
	{"ChooseStrategy", choose_strategy},
	{"ChooseSystem", choose_system},
	{"ChooseTechnology", choose_technology},
	{"ContinueBattle", combat_continue},
	{"Exhaust", exhaust},
	{"EndTurn", end_turn},
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