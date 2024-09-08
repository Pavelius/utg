#include "answers.h"
#include "army.h"
#include "card.h"
#include "condition.h"
#include "deck.h"
#include "filter.h"
#include "list.h"
#include "math.h"
#include "pathfind.h"
#include "pushvalue.h"
#include "planet.h"
#include "planet_trait.h"
#include "player.h"
#include "rand.h"
#include "script.h"
#include "strategy.h"
#include "system.h"
#include "troop.h"
#include "unit.h"
#include "unit_type.h"

static char		sb_temp[512];
static stringbuilder sb(sb_temp);
static entitya	querry, onboard;
static int		last_value;
static int		choose_options;
static bool		choose_stop;
static void*	choose_result;
static const char* choose_id;
static indicator_s command_tokens[] = {TacticToken, FleetToken, StrategyToken};

struct commandstack : variants {
	commandstack() { set(script_begin, script_end - script_begin); }
	void		restore() { script_begin = begin(); script_end = end(); }
};

static bool is_mecatol_rex(const void* object) {
	return object == bsdata<systemi>::elements
		|| object == bsdata<planeti>::elements;
}

static bool filter_cruiser_or_destroyer(const void* object) {
	auto index = bsdata<playeri>::source.indexof(player);
	auto p = (troop*)object;
	auto pu = p->getunit();
	return pu == (uniti*)(bsdata<prototype>::elements + 4) // Destroyer
		|| pu == (uniti*)(bsdata<prototype>::elements + 6); // Cruiser
}

static bool filter_tag(const void* object) {
	return ((entity*)object)->is((tag_s)last_filter->param);
}

static bool filter_activated(const void* object) {
	return ((entity*)object)->is((tag_s)bsdata<playeri>::source.indexof(player));
}

static bool is_ingame(const void* object) {
	auto p = ((entity*)object)->getsystem();
	if(!p)
		return false;
	return p->index != pathfind::Blocked;
}

static bool join_systems(const void* object, int index) {
	auto p = ((entity*)object)->getsystem();
	if(!p)
		return false;
	return ((collectiona*)index)->have(p);
}

static bool filter_wormhole(const void* object) {
	auto p = ((entity*)object)->getsystem();
	if(!p)
		return false;
	return p->special == WormholeAlpha || p->special == WormholeBeta;
}

static bool filter_home_system(const void* object) {
	auto p = ((entity*)object)->getsystem();
	if(!p)
		return false;
	return p->home == player;
}

static bool filter_home_system_any(const void* object) {
	auto p = ((entity*)object)->getsystem();
	if(!p)
		return false;
	return p->home != 0;
}

static bool filter_enemy_ships_system(const void* object) {
	auto system = ((entity*)object)->getsystem();
	if(!system)
		return false;
	for(auto& e : bsdata<troop>()) {
		if(e.player == player)
			continue;
		if(e.getsystem() == system)
			return true;
	}
	return false;
}

static bool filter_free(const void* object) {
	if(bsdata<strategyi>::have(object)) {
		for(auto p : players) {
			if(p && p->strategy == object)
				return false;
		}
	}
	return true;
}

static playeri* find_player(const strategyi& e) {
	for(auto p : players) {
		if(p->strategy == &e)
			return p;
	}
	return 0;
}

static int compare_initiative(const void* v1, const void* v2) {
	auto p1 = *((playeri**)v1);
	auto p2 = *((playeri**)v2);
	auto i1 = p1->getinitiative();
	auto i2 = p2->getinitiative();
	if(i1 != i2)
		return i1 - i2;
	return getbsi(p1) - getbsi(p2);
}

static void sort_by_initiative() {
	querry.sort(compare_initiative);
}

static void human_focusing() {
	if(!player->ishuman())
		return;
	auto system_focus = querry[0]->getsystem();
	if(system_focus)
		system_focus->focusing();
}

static void add_choose_options() {
	if(choose_options > 0)
		sb.adds(getnm("ChooseOptions"), choose_options);
}

static void add_entity(entity* p) {
	auto planet = p->getplanet();
	if(planet)
		an.add(p, "%1 (%2)", p->getname(), planet->getname());
	else
		an.add(p, p->getname());
}

static void select_answers(entitya& source) {

}

static void addscript(const char* id) {
	auto p = bsdata<script>::find(id);
	if(p)
		an.add(p, getnm(p->id));
}

static void makewave(pathfind::indext start) {
	pathfind::clearpath();
	block_move();
	block_enemy(player);
	pathfind::makewave(start);
	block_enemy(player);
}

void entitya::addreach(const systemi* system, int range) {
	pathfind::clearpath();
	block_move();
	pathfind::makewave(system->index);
	for(auto i = 0; i < pathfind::maxcount; i++) {
		auto move = pathfind::getmove(i);
		if(move == pathfind::Blocked || move > range)
			continue;
		auto ps = find_system(i);
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
		p1->location = last_system;
		ships.remove(0);
		result++;
	}
	return result;
}

static void standart_answers() {
	for(auto& e : bsdata<componenti>()) {
		if(e.count)
			continue;
		if(!equal(e.trigger, choose_id))
			continue;
		if(!e.isallow())
			continue;
		if(!script_allow(e.use))
			continue;
		an.add(&e, e.getname());
	}
}

static bool standart_apply() {
	if(bsdata<componenti>::have(choose_result))
		script_run(((componenti*)choose_result)->use);
	else if(bsdata<script>::have(choose_result))
		((script*)choose_result)->proc(0);
	else
		return false;
	return true;
}

static void choose_player(const char* cancel_text) {
	pushvalue push_header(answers::header, player->getname());
	pushvalue push_resid(answers::resid, player->id);
	choose_result = an.choose(sb_temp, cancel_text, 1);
}

static void choose_complex(const char* id, const char* cancel, fnevent add_answers, fnevent apply_answers, fnevent ai_answers) {
	pushvalue push_id(choose_id, id);
	pause();
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
		choose_player(cancel_text);
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
	pause();
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
	if(!script_allow(e.secondary))
		return;
	if(player->ishuman()) {
		if(!yesno(getnm("ApplySecondanaryStrategy"), e.getname()))
			return;
		script_run(e.secondary);
	} else {
		if(d100() < 50)
			script_run(e.secondary);
	}
}

static void apply_primary_ability(strategyi& e) {
	player->use_strategy = true;
	script_run(e.primary);
	auto push_player = player;
	for(auto pa : players) {
		if(pa == push_player)
			continue;
		player = pa;
		apply_secondanary_ability(e);
	}
	player = push_player;
}

static void ask_choose_action() {
	if(player->strategy && !player->use_strategy)
		an.add(player->strategy, player->strategy->getname());
}
static void apply_choose_action() {
	if(bsdata<strategyi>::have(choose_result)) {
		auto p = (strategyi*)choose_result;
		if(player->strategy == p)
			apply_primary_ability(*p);
	}
}
void choose_action(int bonus) {
	if(player->pass_action_phase)
		return;
	choose_complex("ChooseAction", 0, ask_choose_action, apply_choose_action, 0);
}

static void ask_movement() {
	if(!last_system)
		return;
	makewave(last_system->index);
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
		pushvalue push(last_troop, (troop*)choose_result);
		script_run("ChooseMoveOption", 0);
	}
}
static void ai_movement() {
	entitya ships;
	ships.select_answers();
	for(auto p : ships) {
		auto capacity = p->get(Capacity);
		auto player = p->player;
		auto system = p->getsystem();
		capacity -= ai_load(player, system, Ships, capacity);
		capacity -= ai_load(player, system, GroundForces, capacity);
		p->location = last_system;
	}
	update_ui();
}
static void choose_movement(int bonus) {
	onboard.clear();
	choose_until_stop("ChooseMove", "EndMovement", ask_movement, apply_movement, ai_movement);
	update_control();
}

static void apply_value(indicator_s v, int value) {
	if(value == 0) {
		// Simple get value
		last_value = player->get(v);
		return;
	} else if(value == 100)
		value = last_value;
	else if(value == -100)
		value = -last_value;
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
		break;
	}
}

static void change_to_system(int bonus) {
	auto ps = querry.begin(), pb = ps;
	auto pe = querry.end();
	while(pb < pe) {
		auto p = (*pb++)->getsystem();
		if(p)
			*ps++ = p;
	}
	querry.count = ps - querry.begin();
	querry.distinct();
}

static void change_to_planet(int bonus) {
	auto ps = querry.begin(), pb = ps;
	auto pe = querry.end();
	while(pb < pe) {
		auto p = (*pb++)->getplanet();
		if(p)
			*ps++ = p;
	}
	querry.count = ps - querry.begin();
	querry.distinct();
}

static bool filter_player(const void* object) {
	return ((entity*)object)->player == player;
}

static bool filter_speaker(const void* object) {
	return ((entity*)object)->player == speaker;
}

static void no_mecatol_rex(int bonus) {
	querry.match(is_mecatol_rex, false);
}

static void select_variant(int bonus) {
	querry.clear();
	variant v = *script_begin++;
	if(v.iskind<varianti>())
		querry.select(*bsdata<varianti>::elements[v.value].source);
}

static void select_planets(int bonus) {
	querry.select(bsdata<planeti>::source, filter_player, bonus >= 0);
}

static void group_location() {
	for(auto& e : querry)
		e = e->location;
	querry.distinct();
}

static void select_troops(int bonus) {
	querry.select(bsdata<troop>::source, filter_player, bonus >= 0);
}

static void select_troop(int bonus) {
	querry.clear();
	for(auto& e : bsdata<troop>()) {
		if(!e)
			continue;
		if(e.getsystem() == last_system)
			querry.add(&e);
	}
}

static void select_troop_home(int bonus) {
	querry.clear();
	auto system = player->gethome();
	for(auto& e : bsdata<troop>()) {
		if(!e)
			continue;
		if(e.getsystem() == system)
			querry.add(&e);
	}
}

static void join_planets_by_systems(int bonus) {
	auto systems = querry;
	querry.select(bsdata<planeti>::source, join_systems, (int)&systems, true);
}

static void join_troop_by_systems(int bonus) {
	auto systems = querry;
	querry.select(bsdata<troop>::source, join_systems, (int)&systems, true);
}

static void select_systems(int bonus) {
	querry.select(bsdata<systemi>::source, is_ingame, true);
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

static void sort_by_speaker() {
	if(!speaker)
		return;
	auto maximum = querry.getcount();
	for(size_t i = 0; i < maximum; i++) {
		auto p = querry[0];
		if(p == speaker)
			break;
		memmove(querry.data, querry.data + 1, (maximum - 1) * sizeof(querry.data[0]));
		querry.data[maximum - 1] = p;
	}
}

static void select_players(int bonus) {
	querry.clear();
	for(auto p : players) {
		if(p)
			querry.add(p);
	}
	switch(bonus) {
	case 1: sort_by_speaker(); break;
	case 2: sort_by_initiative(); break;
	default: break;
	}
}

static void choose_planet(int bonus) {
	last_planet = (planeti*)querry.choose(0, 0, 2);
	last_planet->getsystem()->focusing();
}

static void choose_troop(int bonus) {
	last_troop = (troop*)querry.choose(0, 0, 2);
	last_troop->getsystem()->focusing();
}

static void choose_player(int bonus) {
	player = (playeri*)querry.choose(0);
}

static void choose_system(int bonus) {
	human_focusing();
	last_system = (systemi*)querry.choose(0);
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
	querry.match(last_indicator, true);
	auto total = querry.getsummary(last_indicator);
	if(total < choose_options)
		return;
	for(auto p : querry)
		an.add(p, getnm("PayAnswer"), getnm(p->id), p->get(last_indicator));
}
static void apply_pay() {
	if(bsdata<planeti>::have(choose_result)) {
		auto p = (planeti*)choose_result;
		choose_options -= p->get(last_indicator);
		p->exhaust();
	}
}
static void convert_resource(indicator_s need, indicator_s currency, int rate) {
	pushvalue push_options(choose_options, getrate(need, currency, rate));
	pushvalue push_currency(last_indicator, currency);
	choose_pay("ChoosePay", "DoNotPay", ask_convert_resource, apply_pay, 0, rate);
}
static void complex_pay(indicator_s need, int rate) {
	pushvalue push_options(choose_options, rate);
	pushvalue push_currency(last_indicator, need);
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
		if(ps != last_system)
			continue;
		if(e.location == last_planet) {
			if(player->ishuman())
				an.add(&e, "%1 (%Cancel)", e.getname());
		} else if(!bsdata<planeti>::have(e.location))
			an.add(&e, e.getname());
	}
}
static void apply_invasion() {
	if(bsdata<troop>::have(choose_result)) {
		auto p = (troop*)choose_result;
		if(p->location == last_planet)
			p->location = last_planet->getsystem();
		else
			p->location = last_planet;
	}
}
static void ai_invasion() {
	auto system = last_system;
	// Select all planets in a system
	entitya planets;
	planets.selectplanets(system);
	if(!planets)
		return;
	auto planet_count = planets.getcount();
	// Select all ground units in a space
	entitya ground;
	ground.select(player, system);
	ground.match(GroundForces, true);
	if(!ground)
		return;
	auto average = (ground.getcount() + planet_count - 1) / planet_count;
	if(average < 1)
		average = 1;
	for(auto planet : planets) {
		size_t count = 0;
		for(auto p : ground) {
			if(count >= average)
				break;
			if(p->location != system)
				continue;
			p->location = planet;
			count++;
		}
	}
	update_ui();
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
		if(e.location != last_system)
			continue;
		an.add(&e, e.getname());
	}
}
static void apply_invasion_planet() {
	if(bsdata<planeti>::have(choose_result)) {
		auto push_last = last_planet;
		auto push_result = choose_result;
		last_planet = (planeti*)choose_result;
		script_run("ChooseInvasion", 0);
		choose_result = push_result;
		last_planet = push_last;
		update_control();
		update_ui();
	}
}
static void choose_invasion_planet(int bonus) {
	choose_until_stop("ChooseInvasionPlanet", "EndInvasion", ask_invasion_planet, apply_invasion_planet, 0);
}

static void ask_move_options() {
	auto ship = last_troop;
	auto system = ship->getsystem();
	size_t capacity = ship->get(Capacity);
	sb.clear();
	sb.add(getnm("ChooseMoveOption"), ship->getname(), last_system->getname());
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
	if(!last_troop)
		return;
	auto production = last_troop->getproduction();
	if(!production)
		return;
	auto player = last_troop->player;
	auto resources = player->getplanetsummary(Resources);
	auto goods = player->get(TradeGoods);
	auto production_count = onboard.getsummary(CostCount);
	auto total_cost = onboard.getproductioncost();
	auto total_production = production - production_count;
	auto total_resources = goods + resources - total_cost;
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
	auto pu = last_troop;
	onboard.clear();
	choose_until_stop("ChooseProduction", "EndBuild", ask_production, apply_production, 0);
	complex_pay(Resources, onboard.getproductioncost());
	for(auto p : onboard)
		pu->produce((uniti*)p);
	update_ui();
}

static void ask_dock() {
	for(auto& e : bsdata<troop>()) {
		if(e.player != player || !e.get(Production) || e.getsystem() != last_system)
			continue;
		add_entity(&e);
	}
}
static void apply_dock() {
	if(bsdata<troop>::have(choose_result))
		last_troop = (troop*)choose_result;
}
static void choose_dock(int bonus) {
	choose_complex("ChooseDock", 0, ask_dock, apply_dock, 0);
}

static bool filter_add_unit_maximum(const void* object) {
	auto p = (uniti*)object;
	auto maximum_count = p->abilities[Reinforcement];
	if(!maximum_count)
		return true;
	auto count = troop_count(player, last_unit);
	return count < maximum_count;
}

static bool filter_add_unit(const void* object) {
	if(!filter_add_unit_maximum(object))
		return false;
	auto p = (uniti*)object;
	if(p->type == Structures) {
		auto maximum_count = p->abilities[MaximumInOneLocation];
		for(auto& e : bsdata<planeti>()) {
			if(e.player != player)
				continue;
			entitya source;
			source.select(player, &e);
			if(source.getsummary(p) < maximum_count)
				return true;
		}
		return false;
	}
	return true;
}

static bool filter_add_unit_planet(const void* object) {
	auto maximum_count = last_unit->abilities[MaximumInOneLocation];
	if(!maximum_count)
		return true;
	entitya source;
	source.select(player, (planeti*)object);
	return source.getsummary(last_unit) < maximum_count;
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
	sb.adds("(%-Round [%1i])", army::round);
	addscript("ContinueBattle");
	if(player->ishuman())
		addscript("RetreatBattle");
}
static void choose_combat_option(int bonus) {
	choose_complex("ChooseCombatOption", 0, ask_combat_option, 0, 0);
}

static void add_strategy(int bonus) {
	if(bonus < 0)
		player->strategy = 0;
	else
		player->strategy = last_strategy;
}

static void add_querry_unit(int bonus) {
	if(bonus >= 0)
		querry.add(last_unit);
	else
		querry.remove(last_unit);
}

static void add_unit(int bonus) {
	last_unit->placement(bonus);
}

static void set_value(int bonus) {
	last_value = bonus;
}

static void add_neighboring(int bonus) {
	querry.distinct();
	for(auto pe : querry) {
		auto p = pe->getsystem();
		if(!p)
			continue;
		auto need_correct = false;
		for(auto d = 0; d < pathfind::maxcount; d++) {
			auto ni = pathfind::to(p->index, d);
			if(ni == pathfind::Blocked)
				continue;
			auto p1 = find_system(ni);
			if(!p1)
				continue;
			need_correct = true;
			querry.add(p1);
		}
		if(need_correct)
			querry.distinct();
	}
}

static void activate_system(int bonus) {
	if(last_system) {
		last_system->focusing();
		last_system->setactivate(player, bonus >= 0);
	}
}

static bool filter_system(const void* object) {
	return ((entity*)object)->getsystem() == last_system;
}

static void filter_controled(int bonus) {
	pathfind::clearpath();
	block_move();
	mark_zero_cost(player);
	pathfind::makewavex();
	querry.matchrange(iabs(bonus), bonus >= 0);
}

static void focus_home_system(int bonus) {
	if(answers::interactive)
		player->gethome()->focusing();
}

static void exhaust(int bonus) {
	if(last_planet)
		last_planet->exhaust();
}

static bool filter_trait(const void* object) {
	return ((entity*)object)->gettrait() == last_filter->param;
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

static bool filter_speciality(const void* object) {
	return ((entity*)object)->getspeciality() == last_filter->param;
}

static bool filter_ability(const void* object) {
	return ((entity*)object)->get((ability_s)last_filter->param) > 0;
}

static bool filter_indicator(const void* object) {
	return ((entity*)object)->get((ability_s)last_filter->param) > 0;
}

static bool filter_active_player(const void* object) {
	return ((entity*)object)->getplayer() == player;
}

static void add_speaker(int bonus) {
	speaker = player;
}

static void add_system(int bonus) {
	if(bonus >= 0)
		querry.add(last_system);
	else
		querry.remove(last_system);
}

static void redistribute_command_tokens(int bonus) {
}

static void research_technology(int bonus) {
	choose_technology(0);
}

static void score_objective(int bonus) {
}

static bool if_control_mecatol_rex(int bonus) {
	return bsdata<planeti>::elements[0].player == player;
}

static void if_able(int bonus) {
}

static bool if_play_strategy(int bonus) {
	return (player->use_strategy==(bonus>=0));
}

static void secret_objective(int bonus) {
}

static void end_turn(int bonus) {
	choose_stop = true;
}

static void querry_count(int bonus) {
	last_value = querry.getcount() + bonus;
}

static void querry_shuffle(int bonus) {
	querry.shuffle();
	if(bonus > 0)
		querry.top(bonus);
}

static void filter_move(int bonus) {
	querry.matchmove(0, bonus >= 0);
}

static void pass_turn(int bonus) {
	player->pass_action_phase = (bonus != -1);
}

static void cancel_order(int counter) {
	onboard.clear();
}

static void move_ship(int bonus) {
	last_troop->location = last_system;
	for(auto p : onboard)
		p->location = last_system;
	update_ui();
	choose_stop = true;
}

static entity* find_reaction_entity(const char* id, const playeri* need_player, const playeri* exclude_player) {
	for(auto& e : bsdata<entity>()) {
		if(e.player != need_player)
			continue;
		if(e.player == exclude_player)
			continue;
		auto p = e.getcard();
		if(!p)
			continue;
		if(!equal(p->id, id))
			continue;
		if(!equal(p->trigger, "Reaction"))
			continue;
		return &e;
	}
	return 0;
}

bool reaction(const char* id, const playeri* need_player, const playeri* exclude_player, ...) {
	auto pe = find_reaction_entity(id, need_player, exclude_player);
	if(!pe)
		return false;
	if(pe->player->ishuman()) {
		if(!pe->player->askv(id, str("Ask%1"), xva_start(exclude_player)))
			return false;
	} else {
		if(d100() < 15)
			return false;
	}
	if(!equal(id, "Sabotage")) {
		if(reaction("Sabotage", 0, pe->player))
			return false;
	}
	pe->player->sayv(str("Say%1"), xva_start(exclude_player));
	pe->discard();
	return true;
}

static void choose_event(answers& an, const char* format) {
	if(player->ishuman()) {
		pushvalue push_header(answers::header, player->getname());
		pushvalue push_resid(answers::resid, player->id);
		choose_result = an.choose(format, getnm("Cancel"), 1);
	} else
		choose_result = an.random();
}

void playeri::event(const char* id) {
	pushvalue push_player(player, this);
	answers an;
	for(auto& e : bsdata<entity>()) {
		if(e.player != player)
			continue;
		auto p = e.getcard();
		if(!p)
			continue;
		if(!equal(p->trigger, id))
			continue;
		if(!script_allow(p->use))
			continue;
		an.add(p, p->getname());
	}
	if(an) {
		choose_event(an, 0);
		standart_apply();
	}
}

static bool allow_continue_execute() {
	while(script_begin < script_end) {
		if(!script_allow(*script_begin++))
			return false;
	}
	return true;
}

static void continue_execute() {
	while(script_begin < script_end)
		script_run(*script_begin++);
}

static void push_player(int bonus) {
	auto push_last = player;
	continue_execute();
	player = push_last;
}

static void for_each_player(int bonus) {
	auto push_last = player;
	auto v = *script_begin++;
	entityd push = querry;
	for(auto p : push) {
		player = static_cast<playeri*>(p);
		script_run(v);
	}
	player = push_last;
}

static bool if_all_players(int bonus) {
	auto push_last = player;
	auto v = *script_begin++;
	auto result = bonus >= 0;
	for(auto p : players) {
		player = static_cast<playeri*>(p);
		if(!script_allow(v)) {
			result = !result;
			break;
		}
	}
	player = push_last;
	return result;
}

static bool if_one_player(int bonus) {
	auto push_last = player;
	auto v = *script_begin++;
	auto result = (bonus < 0);
	for(auto p : players) {
		player = static_cast<playeri*>(p);
		if(script_allow(v)) {
			result = !result;
			break;
		}
	}
	player = push_last;
	return result;
}

static void for_each_troop(int bonus) {
	auto push_troop = last_troop;
	auto v = *script_begin++;
	entityd push = querry;
	for(auto p : push) {
		last_troop = (troop*)p;
		script_run(v);
	}
	last_troop = push_troop;
}

static void for_each_planet(int bonus) {
	auto v = *script_begin++;
	auto push_last = last_planet;
	entityd push = querry;
	for(auto p : push) {
		last_planet = p->getplanet();
		if(last_planet)
			script_run(v);
	}
	last_planet = push_last;
}

static void show_action_cards(int bonus) {
}

static void show_tech(int bonus) {
}

static void apply_header(int bonus) {
	auto push_id = choose_id;
	auto push_header = answers::header;
	auto push_res = answers::resid;
	if(last_list) {
		choose_id = last_list->id;
		answers::header = getnm(last_list->id);
	}
	if(player)
		answers::resid = player->id;
	continue_execute();
	answers::resid = push_res;
	answers::header = push_header;
	choose_id = push_id;
}

static bool allow_apply_header(int bonus) {
	auto push_id = choose_id;
	auto push_header = answers::header;
	auto push_res = answers::resid;
	if(last_list) {
		choose_id = last_list->id;
		answers::header = getnm(last_list->id);
	}
	if(player)
		answers::resid = player->id;
	if(!allow_continue_execute())
		return false;
	answers::resid = push_res;
	answers::header = push_header;
	choose_id = push_id;
	return true;
}

static void entity_name(const void* object, stringbuilder& sb) {
	auto p = (entity*)object;
	sb.add(p->getname());
}

static void header_many_choose(stringbuilder& sb) {
	const char* pd = 0;
	if(onboard)
		pd = getnme(ids(choose_id, "AskChoosed"));
	if(!pd)
		pd = getnme(ids(choose_id, "Ask"));
	if(pd) {
		sb.addn("###%1", pd);
		if(choose_options) {
			auto count = choose_options - onboard.getcount();
			if(count > 0)
				sb.adds(getnm("ChooseOptions"), count);
		}
	}
	for(auto p : onboard)
		sb.addn(p->getname());
}

static void apply_many_choose() {
	if(bsdata<playeri>::have(choose_result)
		|| bsdata<planeti>::have(choose_result)
		|| bsdata<troop>::have(choose_result)
		|| bsdata<strategyi>::have(choose_result)
		|| bsdata<uniti>::have(choose_result) || bsdata<prototype>::have(choose_result)
		|| bsdata<systemi>::have(choose_result)
		|| bsdata<abilityi>::have(choose_result)) {
		auto find_index = onboard.find(choose_result);
		if(find_index == -1)
			onboard.add(choose_result);
	} else
		((fnevent)choose_result)();
}

static void apply_choose() {
	if(bsdata<strategyi>::have(choose_result))
		last_strategy = (strategyi*)choose_result;
	else if(bsdata<planeti>::have(choose_result))
		last_planet = (planeti*)choose_result;
	else if(bsdata<systemi>::have(choose_result))
		last_system = (systemi*)choose_result;
	else if(bsdata<uniti>::have(choose_result) || bsdata<prototype>::have(choose_result))
		last_unit = (uniti*)choose_result;
	else if(bsdata<playeri>::have(choose_result))
		player = (playeri*)choose_result;
}

static void clear_onboard() {
	onboard.clear();
}

static void apply_onboard_to_querry() {
	querry = onboard;
	choose_stop = true;
}

static void choose_many(int maximum_count, fnstatus getname, fnprint getheader) {
	if(!choose_id || !answers::console)
		return;
	if(!player->ishuman()) {
		auto ps = bsdata<script>::find(ids(choose_id, "AI"));
		if(ps)
			ps->proc(maximum_count);
		else {
			querry.shuffle();
			querry.top(maximum_count);
		}
		return;
	}
	auto push_stop = choose_stop; choose_stop = false;
	auto push_options_count = choose_options; choose_options = maximum_count;
	choose_result = 0;
	auto cancel = getnme(ids(choose_id, "Cancel"));
	onboard.clear();
	while(!choose_stop) {
		an.clear();
		int count = onboard.getcount();
		if(!choose_options || choose_options > count) {
			for(auto p : querry) {
				if(onboard.find(p) != -1)
					continue;
				sb.clear(); getname(p, sb);
				an.add(p, sb_temp);
			}
		}
		if(onboard) {
			an.add(clear_onboard, getnm("ClearSelection"));
			if(!maximum_count || count >= maximum_count) {
				auto pd = getnme(ids(choose_id, "Apply"));
				if(!pd)
					pd = getnm("Continue");
				an.add(apply_onboard_to_querry, pd);
			}
		}
		sb.clear(); getheader(sb);
		auto push_promp = answers::prompt; answers::prompt = sb_temp;
		choose_result = an.choose(0, cancel);
		answers::prompt = push_promp;
		if(!choose_result) {
			script_stop();
			break;
		}
		apply_many_choose();
	}
	choose_options = push_options_count;
	choose_stop = push_stop;
}

static void header_single_choose(stringbuilder& sb) {
	auto pd = getnme(ids(choose_id, "Ask"));
	if(pd)
		sb.addn("###%1", pd);
}

static void choose_single(fnstatus getname, fnprint getheader) {
	if(!choose_id || !answers::console)
		return;
	if(!querry)
		return;
	choose_result = 0;
	if(!player->ishuman()) {
		auto ps = bsdata<script>::find(ids(choose_id, "AI"));
		if(ps)
			ps->proc(1);
		else
			choose_result = querry.random();
		apply_choose();
		return;
	} else
		human_focusing();
	an.clear();
	for(auto p : querry) {
		sb.clear(); getname(p, sb);
		an.add(p, sb_temp);
	}
	standart_answers();
	auto cancel = getnme(ids(choose_id, "Cancel"));
	sb.clear(); getheader(sb);
	auto push_promp = answers::prompt; answers::prompt = sb_temp;
	choose_result = an.choose(0, cancel);
	answers::prompt = push_promp;
	if(!choose_result)
		script_stop();
	else
		apply_choose();
}

static void choose_many(int bonus) {
	choose_many(bonus, entity_name, header_many_choose);
}

static void choose_single(int bonus) {
	choose_single(entity_name, header_single_choose);
}

static bool if_non_zero(int bonus) {
	return last_value > 0;
}
static void conditional(int bonus) {
	if(!last_script->test(bonus))
		script_stop();
}

static void do_break(int bonus) {
	script_stop();
	choose_stop = true;
}

static void conditional_break(int bonus) {
	if(!last_script->test(bonus))
		do_break(0);
}

static void do_repeat(int bonus) {
	commandstack stack;
	auto push_stop = choose_stop; choose_stop = false;
	while(!choose_stop) {
		stack.restore();
		continue_execute();
	}
	choose_stop = push_stop;
}

static void do_switch(int bonus) {
	commandstack stack;
	auto i = script_end - script_begin;
	if(bonus > i)
		bonus = i;
	if(last_value >= bonus)
		last_value = bonus - 1;
	script_run(script_begin[last_value]);
	stack.restore();
	script_begin += bonus;
}

static bool if_winner(int bonus) {
	return player==game_winner;
}

static bool if_pass(int bonus) {
	return player->pass_action_phase == (bonus>=0);
}

static void clear_querry(int bonus) {
	querry.clear();
}

static bool common_select(int bonus) {
	last_script->proc(bonus);
	return querry.getcount() > 0;
}

static bool common_action(int bonus) {
	last_script->proc(bonus);
	return true;
}

static bool allow_choose_action(int bonus) {
	return !player->pass_action_phase;
}

static bool common_choose(int bonus) {
	script_stop();
	return querry.getcount() > 0;
}

static bool not_impotant(int bonus) {
	script_stop();
	return true;
}

template<> void fnscript<decki>(int index, int bonus) {
	auto pd = bsdata<decki>::elements + index;
	for(auto i = 0; i < bonus; i++) {
		auto p = pd->cards.pick();
		if(p)
			p->player = player;
	}
}

template<> void fnscript<filteri>(int index, int bonus) {
	last_filter = bsdata<filteri>::elements + index;
	if(last_filter->custom)
		last_filter->custom(bonus);
	else
		querry.match(last_filter->proc, bonus >= 0);
}
template<> bool fntest<filteri>(int index, int bonus) {
	fnscript<filteri>(index, bonus);
	return querry.getcount() > 0;
}

template<> bool fntest<indicatori>(int index, int bonus) {
	return (player->get((indicator_s)index) + bonus) >= 0;
}
template<> void fnscript<indicatori>(int index, int bonus) {
	apply_value((indicator_s)index, bonus);
}

template<> void fnscript<uniti>(int index, int bonus) {
	last_unit = bsdata<uniti>::elements;
	if(player)
		last_unit = bsdata<prototype>::elements[player->getindex()].units + index;
	else
		last_unit = bsdata<uniti>::elements + index;
}

void combat_reatreat(int bonus);
void combat_continue(int bonus);

BSDATA(filteri) = {
	{"FilterActivated", filter_activated},
	{"FilterActivePlayer", filter_active_player},
	{"FilterAddUnit", filter_add_unit},
	{"FilterAddUnitPlanet", filter_add_unit_planet},
	{"FilterAnyHomeSystem", filter_home_system_any},
	{"FilterBombardment", filter_ability, 0, Bombardment},
	{"FilterCommodities", filter_indicator, 0, Commodities},
	{"FilterControled", 0, filter_controled},
	{"FilterCultural", filter_trait, 0, Cultural},
	{"FilterCruiserOrDestroyer", filter_cruiser_or_destroyer},
	{"FilterEnemyShipSystem", filter_enemy_ships_system},
	{"FilterExhaust", filter_tag, 0, Exhaust},
	{"FilterFree", filter_free},
	{"FilterIndustrial", filter_trait, 0, Industrial},
	{"FilterHasardous", filter_trait, 0, Hazardous},
	{"FilterHomeSystem", filter_home_system},
	{"FilterMoveStop", 0, filter_move},
	{"FilterPlanetTrait", filter_trait, 0, NoTrait},
	{"FilterPlayer", filter_player},
	{"FilterProduction", filter_ability, 0, Production},
	{"FilterSpeaker", filter_speaker},
	{"FilterSystem", filter_system},
	{"FilterRedTechnology", filter_speciality, 0, Red},
	{"FilterNoTechnology", filter_speciality, 0, NoTech},
	{"FilterWormhole", filter_wormhole},
};
BSDATAF(filteri);
BSDATA(script) = {
	{"ActivateSystem", activate_system},
	{"AddNeighboring", add_neighboring},
	{"AddSpeaker", add_speaker},
	{"AddStrategy", add_strategy},
	{"AddUnit", add_unit},
	{"AddUnitQuerry", add_querry_unit},
	{"ApplyHeader", apply_header, allow_apply_header},
	{"Break", do_break},
	{"CancelOrder", cancel_order},
	{"ChangeToPlanet", change_to_planet, common_choose},
	{"ChangeToSystem", change_to_system, common_choose},
	{"ChooseAction", choose_action, allow_choose_action},
	{"ChooseCombatOption", choose_combat_option, common_choose},
	{"ChooseCommandToken", choose_command_token, common_choose},
	{"ChooseDock", choose_dock, common_choose},
	{"ChooseInvasion", choose_invasion, common_choose},
	{"ChooseInvasionPlanet", choose_invasion_planet, common_choose},
	{"ChooseMany", choose_many, common_choose},
	{"ChooseMove", choose_movement, common_choose},
	{"ChooseMoveOption", choose_move_options, common_choose},
	{"ChoosePlanet", choose_planet, common_choose},
	{"ChoosePlayer", choose_player, common_choose},
	{"ChooseProduction", choose_production, common_choose},
	{"ChooseSingle", choose_single, common_choose},
	{"ChooseSystem", choose_system, common_choose},
	{"ChooseTechnology", choose_technology, common_choose},
	{"ChooseTroop", choose_troop, common_choose},
	{"ClearQuerry", clear_querry, common_action},
	{"ContinueBattle", combat_continue},
	{"Exhaust", exhaust},
	{"EndTurn", end_turn},
	{"FocusHomeSystem", focus_home_system},
	{"ForEachPlanet", for_each_planet},
	{"ForEachPlayer", for_each_player},
	{"ForEachTroop", for_each_troop},
	{"IfAllPlayers", conditional_break, if_all_players},
	{"IfControlMecatolRex", conditional, if_control_mecatol_rex},
	{"IfOnePlayer", conditional_break, if_one_player},
	{"IfPass", conditional, if_pass},
	{"IfPlayStrategy", conditional, if_play_strategy},
	{"IfNonZero", conditional, if_non_zero},
	{"IfWinner", conditional, if_winner},
	{"JoinPlanetsBySystems", join_planets_by_systems},
	{"JoinTroopsBySystems", join_troop_by_systems},
	{"MoveShip", move_ship},
	{"NoMecatolRex", no_mecatol_rex},
	{"NotImpotant", script_none, not_impotant},
	{"PayCommandTokens", pay_command_tokens},
	{"PassTurn", pass_turn},
	{"PushPlayer", push_player},
	{"QuerryCount", querry_count},
	{"QuerryShuffle", querry_shuffle},
	{"RedistributeCommandTokens", redistribute_command_tokens},
	{"Repeat", do_repeat},
	{"ReplenishCommodities", replenish_commodities},
	{"ResearchTechnology", research_technology},
	{"ScorePublicObjective", score_objective},
	{"SecretObjective", secret_objective},
	{"Select", select_variant, common_select},
	{"SelectPlanets", select_planets, common_select},
	{"SelectPlayers", select_players, common_select},
	{"SelectSystems", select_systems, common_select},
	{"SelectSystemReach", select_system_reach, common_select},
	{"SelectTroopActive", select_troop, common_select},
	{"SelectTroopHome", select_troop_home, common_select},
	{"SelectTroops", select_troops, common_select},
	{"SetValue", set_value},
	{"ShowActionCards", show_action_cards},
	{"ShowTech", show_tech},
	{"Switch", do_switch},
	{"RetreatBattle", combat_reatreat},
};
BSDATAF(script);