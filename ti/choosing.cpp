#include "main.h"

entitya				onboard;
static indicator_s	command_tokens[] = {TacticToken, FleetToken, StrategyToken};

static void clear_onboard() {
	onboard.clear();
}

static playeri* find_player(const strategyi& e) {
	for(auto p : game.players) {
		if(p->strategy == &e)
			return p;
	}
	return 0;
}

static void select_planets(const playeri* player, bool iscontrol) {
	querry.clear();
	querry.select(bsdata<planeti>::source);
	querry.match(player, iscontrol);
}

static void choose_pay(stringbuilder& sb, answers& an) {
	select_planets(playeri::last, true);
	querry.match(Exhaust, false);
	querry.match(game.indicator, true);
	for(auto p : querry)
		an.add(p, getnm("PayAnswer"), getnm(p->id), p->get(game.indicator));
}

static void choose_command_token(stringbuilder& sb, answers& an) {
	for(auto v : command_tokens) {
		auto& e = bsdata<indicatori>::elements[v];
		an.add(&e, getnm(e.id));
	}
}

static void apply_command_token() {
	if(bsdata<indicatori>::have(choosestep::result)) {
		auto v = (indicator_s)bsdata<indicatori>::source.indexof(choosestep::result);
		playeri::last->add(v, 1);
		choosestep::options--;
		choosestep::stop = (choosestep::options <= 0);
	}
}

static void apply_pay() {
	if(bsdata<planeti>::have(choosestep::result)) {
		auto p = (planeti*)choosestep::result;
		choosestep::options -= p->get(game.indicator);
		p->exhaust();
	}
}

static void choose_strategy(stringbuilder& sb, answers& an) {
	for(auto& e : bsdata<strategyi>()) {
		if(find_player(e))
			continue;
		an.add(&e, getnm(e.id));
	}
}

static void apply_strategy() {
	if(bsdata<strategyi>::have(choosestep::result)) {
		auto p = (strategyi*)choosestep::result;
		playeri::last->strategy = p;
		choosestep::stop = true;
	}
}

static void choose_action(stringbuilder& sb, answers& an) {
	auto p = game.active;
	if(!p)
		return;
	if(p->strategy && !p->use_strategy)
		an.add(p->strategy, getnm(p->strategy->getname()));
}

static void apply_action() {
	if(bsdata<strategyi>::have(choosestep::result)) {
		auto p = (strategyi*)choosestep::result;
		if(game.active->strategy == p) {
			script::run(p->primary);
			game.active->use_strategy = true;
		} else
			script::run(p->secondary);
		choosestep::stop = true;
	}
}

static void std_answer(stringbuilder& sb, answers& an) {
	for(auto& e : bsdata<component>()) {
		if(e.trigger != choosestep::last)
			continue;
		if(!e.isallow())
			continue;
		if(!script::isallow(e.use))
			continue;
		an.add(&e, e.getname());
	}
	//for(auto& e : bsdata<actioncardi>()) {
	//	if(e.trigger != trigger)
	//		continue;
	//	if(!script::isallow(e.use))
	//		continue;
	//	an.add(&e, e.getname());
	//}
}

static void makewave(pathfind::indext start) {
	pathfind::clearpath();
	systemi::blockmove();
	systemi::blockenemy(playeri::last);
	pathfind::makewave(start);
	systemi::blockenemy(playeri::last);
}

static void choose_movement(stringbuilder& sb, answers& an) {
	makewave(systemi::active->index);
	for(auto& e : bsdata<troop>()) {
		if(e.player != playeri::last)
			continue;
		auto pu = e.getunit();
		if(!pu)
			continue;
		auto move = e.get(Move);
		if(!move)
			continue;
		auto ps = e.getsystem();
		if(!ps || ps->isactivated(playeri::last))
			continue;
		auto cost_move = pathfind::getmove(ps->index);
		if(cost_move == pathfind::Blocked || move < cost_move)
			continue;
		an.add(&e, "%1 (%2)", e.getname(), e.location->getsystem()->getname());
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

static void ai_choose_movement(answers& an) {
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

static void apply_movement() {
	if(bsdata<troop>::have(choosestep::result)) {
		auto push_last = troop::last;
		troop::last = (troop*)choosestep::result;
		game.focusing(troop::last);
		choosestep::run("ChooseMoveOption");
		troop::last = push_last;
	}
}

static void add_script(answers& an, const char* id) {
	auto p = bsdata<script>::find(id);
	if(p) {
		an.add(p, getnm(p->id));
		return;
	}
	auto ps = bsdata<choosestep>::find(id);
	if(ps) {
		an.add(ps, getnm(ps->id));
		return;
	}
}

static bool allow(const uniti* pu) {
	auto maximum_count = pu->abilities[MaximumInOneLocation];
	if(pu->abilities[MaximumInOneLocation] > 0) {
		if(pu->type == Structures) {
			entitya source;
			source.select(game.active, planeti::last);
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

static void choose_pds_or_dock(stringbuilder& sb, answers& an) {
	add_unit(an, "PDS");
	add_unit(an, "SpaceDock");
}

static void apply_pds_or_dock() {
	if(bsdata<uniti>::have(choosestep::result)) {
		((uniti*)choosestep::result)->placement(1);
		choosestep::stop = true;
	}
}

static void choose_move_options(stringbuilder& sbt, answers& an) {
	auto ship = troop::last;
	auto system = ship->getsystem();
	auto capacity = ship->get(Capacity);
	sbt.clear();
	sbt.add(getnm("ChooseMoveOption"), ship->getname(), systemi::active->getname());
	char temp[260]; stringbuilder sb(temp);
	add_script(an, "MoveShip");
	if(capacity > 0) {
		for(auto& e : bsdata<troop>()) {
			if(e.player != playeri::last || e.getsystem() != system)
				continue;
			if(e.getunit()->type == Structures)
				continue;
			if(e.get(Move) != 0)
				continue;
			sb.clear();
			if(onboard.have(&e)) {
				if(choosestep::human)
					sb.add(getnm("UnboardTroop"), e.getname());
			} else if(onboard.getcount() < capacity)
				sb.add(getnm("BoardTroop"), e.getname());
			else
				continue;
			if(e.location && e.location != system)
				sb.adds("(%1)", e.location->getname());
			an.add(&e, temp);
		}
	}
}

static void apply_move_options() {
	if(bsdata<troop>::have(choosestep::result)) {
		auto p = (troop*)choosestep::result;
		auto index = onboard.find(p);
		if(index == -1)
			onboard.add(p);
		else
			onboard.remove(index, 1);
	}
}

static void choose_invasion(stringbuilder& sb, answers& an) {
	for(auto& e : bsdata<troop>()) {
		if(e.player != playeri::last)
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
		if(bsdata<planeti>::have(e.location))
			continue;
		an.add(&e, e.getname());
	}
}

static void ai_choose_invasion(answers& an) {
	auto player = game.active;
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

static bool have_invasion_units() {
	answers an; char temp[260]; stringbuilder sb(temp);
	choose_invasion(sb, an);
	return an.getcount() != 0;
}

static void apply_invasion() {
	if(bsdata<troop>::have(choosestep::result)) {
		auto p = (troop*)choosestep::result;
		p->location = planeti::last;
		game.updateui();
	}
}

static void choose_invasion_planet(stringbuilder& sb, answers& an) {
	if(!have_invasion_units())
		return;
	for(auto& e : bsdata<planeti>()) {
		if(e.location != systemi::active)
			continue;
		an.add(&e, e.getname());
	}
}

static void end_movement() {
	game.updatecontrol();
}

static void apply_invasion_planet() {
	if(bsdata<planeti>::have(choosestep::result)) {
		auto push_last = planeti::last;
		planeti::last = (planeti*)choosestep::result;
		choosestep::run("ChooseInvasion");
		planeti::last = push_last;
		game.updatecontrol();
	}
}

static void choose_dock(stringbuilder& sb, answers& an) {
	for(auto& e : bsdata<troop>()) {
		if(e.player != game.active || !e.get(Production) || e.getsystem() != systemi::active)
			continue;
		e.add(an);
	}
}

static void apply_dock() {
	if(bsdata<troop>::have(choosestep::result)) {
		troop::last = (troop*)choosestep::result;
		choosestep::stop = true;
	}
}

static void choose_production(stringbuilder& sb, answers& an) {
	if(!troop::last)
		return;
	auto production = troop::last->getproduction();
	if(!production)
		return;
	auto player = troop::last->player;
	auto resources = game.active->getplanetsummary(Resources);
	auto goods = player->get(TradeGoods);
	auto production_count = onboard.getsummary(CostCount);
	auto total_production = production - production_count;
	auto total_resources = goods + resources - onboard.getsummary(Cost);
	if(production_count)
		sb.addn("---");
	for(auto p : onboard) {
		auto pu = (uniti*)p;
		sb.addn("%1 - %Cost [%2i]", p->getname(), pu->getcost());
		if(pu->abilities[CostCount] > 1)
			sb.adds("%Count [%1i]", pu->abilities[CostCount]);
	}
	if(onboard)
		sb.addn("---");
	if(production_count)
		sb.addn(getnm("ProductionTotal"), total_resources, total_production);
	for(auto& e : bsdata<uniti>()) {
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
	if(choosestep::human) {
		if(onboard)
			add_script(an, "CancelOrder");
	}
}

static void apply_production() {
	if(bsdata<uniti>::have(choosestep::result))
		onboard.add((entity*)choosestep::result);
}

static void finish_production() {
	auto pu = troop::last;
	for(auto p : onboard)
		pu->produce((uniti*)p);
	game.updateui();
}

static void std_apply() {
	if(bsdata<component>::have(choosestep::result)) {
		auto p = (component*)choosestep::result;
		script::run(p->use);
	} else if(bsdata<script>::have(choosestep::result)) {
		auto p = (script*)choosestep::result;
		p->proc(0, p->param);
	} else if(bsdata<choosestep>::have(choosestep::result)) {
		auto p = (choosestep*)choosestep::result;
		p->run();
	} else
		return;
	choosestep::applied = true;
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

void choosestep_initialize() {
	choosestep::pstd_answer = std_answer;
	choosestep::pstd_apply = std_apply;
}

BSDATA(choosestep) = {
	{"ChooseAction", choose_action, apply_action},
	{"ChooseProduction", choose_production, apply_production, "EndBuild", 0, clear_onboard, finish_production},
	{"ChooseCommandToken", choose_command_token, apply_command_token},
	{"ChooseDock", choose_dock, apply_dock},
	{"ChooseInvasion", choose_invasion, apply_invasion, "Apply", ai_choose_invasion},
	{"ChooseInvasionPlanet", choose_invasion_planet, apply_invasion_planet, "EndInvasion"},
	{"ChooseMove", choose_movement, apply_movement, "EndMovement", ai_choose_movement, clear_onboard, end_movement},
	{"ChooseMoveOption", choose_move_options, apply_move_options, "CancelMovement"},
	{"ChoosePay", choose_pay, apply_pay},
	{"ChoosePDSorDock", choose_pds_or_dock, apply_pds_or_dock},
	{"ChooseStrategy", choose_strategy, apply_strategy},
};
BSDATAF(choosestep)