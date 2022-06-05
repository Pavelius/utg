#include "main.h"

static indicator_s command_tokens[] = {TacticToken, FleetToken, StrategyToken};

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

static void choose_pay(answers& an) {
	select_planets(playeri::last, true);
	querry.match(Exhaust, false);
	querry.match(game.indicator, true);
	for(auto p : querry)
		an.add(p, getnm("PayAnswer"), getnm(p->id), p->get(game.indicator));
}

static void choose_command_token(answers& an) {
	for(auto v : command_tokens) {
		auto& e = bsdata<indicatori>::elements[v];
		an.add(&e, getnm(e.id));
	}
}

static void apply_command_token() {
	if(bsdata<indicatori>::have(game.result)) {
		auto v = (indicator_s)bsdata<indicatori>::source.indexof(game.result);
		playeri::last->add(v, 1);
	}
}

static void apply_pay() {
	if(bsdata<planeti>::have(game.result)) {
		auto p = (planeti*)game.result;
		game.options -= p->get(game.indicator);
		p->exhaust();
	}
}

static void choose_strategy(answers& an) {
	for(auto& e : bsdata<strategyi>()) {
		if(find_player(e))
			continue;
		an.add(&e, getnm(e.id));
	}
}

static void apply_strategy() {
	if(bsdata<strategyi>::have(game.result)) {
		auto p = (strategyi*)game.result;
		playeri::last->strategy = p;
	}
}

static void choose_action(answers& an) {
	auto p = game.active;
	if(!p)
		return;
	if(p->strategy && !p->use_strategy)
		an.add(p->strategy, getnm(p->strategy->getname()));
}

static void apply_action() {
	if(bsdata<strategyi>::have(game.result)) {
		auto p = (strategyi*)game.result;
		if(game.active->strategy == p) {
			script::run(p->primary);
			game.active->use_strategy = true;
		} else
			script::run(p->secondary);
	}
}

static void choose_standart(answers& an, const choosestep* trigger) {
	for(auto& e : bsdata<component>()) {
		if(e.trigger != trigger)
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

static void block_move_system() {
	for(auto& e : bsdata<systemi>()) {
		if(e.index == pathfind::Blocked)
			continue;
		if(!e.movethrought())
			pathfind::setmove(e.index, pathfind::Blocked);
	}
}

static void block_enemy_system(const playeri* player) {
	for(auto& e : bsdata<systemi>()) {
		if(e.index == pathfind::Blocked)
			continue;
		if(e.player && e.player != player)
			pathfind::setmove(e.index, pathfind::Blocked);
	}
}

static void makewave(pathfind::indext start) {
	pathfind::clearpath();
	block_move_system();
	block_enemy_system(playeri::last);
	pathfind::makewave(start);
	block_enemy_system(playeri::last);
}

static void choose_movement(answers& an) {
	auto p = game.active;
	if(!p)
		return;
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
		auto ps = (systemi*)e.location;
		if(!ps)
			continue;
		auto cost_move = pathfind::getmove(ps->index);
		if(cost_move == pathfind::Blocked || move<cost_move)
			continue;
		an.add(&e, "%1 (%2)", e.getname(), e.location->getsystem()->getname());
	}
}

static void apply_movement() {
	if(bsdata<troop>::have(game.result))
		((troop*)game.result)->movement(systemi::active);
}

static bool apply_standart() {
	if(bsdata<component>::have(game.result)) {
		auto p = (component*)game.result;
		script::run(p->use);
	} else
		return false;
	return true;
}

void choosestep::run() const {
	draw::pause();
	auto push_header = answers::header;
	if(playeri::last)
		answers::header = playeri::last->getname();
	char temp[260]; gamestring sb(temp); answers an;
	auto infinite = (game.options == -100);
	while(infinite || game.options > 0) {
		sb.clear();
		sb.add(getnm(id));
		if(!infinite)
			sb.adds(getnm("ChooseOptions"), game.options);
		an.clear(); panswer(an);
		choose_standart(an, this);
		const char* cancel_text = 0;
		if(cancel)
			cancel_text = getnm(cancel);
		if(game.active->ishuman())
			game.result = an.choose(temp, cancel_text);
		else
			game.result = an.random();
		if(!game.result)
			break;
		if(!apply_standart()) {
			if(papply)
				papply();
		}
		game.options--;
	}
	draw::pause();
	answers::header = push_header;
}

void choosestep::run(const char* id) {
	auto p = bsdata<choosestep>::find(id);
	if(p)
		p->run();
}

BSDATA(choosestep) = {
	{"ChooseAction", choose_action, apply_action},
	{"ChooseCommandToken", choose_command_token, apply_command_token},
	{"ChooseMove", choose_movement, apply_movement, "EndMovement"},
	{"ChoosePay", choose_pay, apply_pay},
	{"ChooseStrategy", choose_strategy, apply_strategy},
};
BSDATAF(choosestep)