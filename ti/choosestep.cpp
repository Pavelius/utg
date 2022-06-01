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
		game.options--;
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
}

static void choose_standart(answers& an, const choosestep* step) {
}

static bool apply_standart() {
	return false;
}

void choosestep::run() const {
	char temp[260];
	gamestring sb(temp);
	answers an;
	draw::pause();
	while(game.options > 0) {
		sb.clear();
		sb.add(getnm(id));
		sb.adds(getnm("ChooseOptions"), game.options);
		an.clear(); panswer(an);
		choose_standart(an, this);
		game.result = an.choose(temp);
		if(!game.result)
			break;
		if(apply_standart())
			continue;
		if(papply)
			papply();
	}
	draw::pause();
}

void choosestep::run(const char* id) {
	auto p = bsdata<choosestep>::find(id);
	if(p)
		p->run();
}

BSDATA(choosestep) = {
	{"AsAction", choose_action, apply_action},
	{"ChooseCommandToken", choose_command_token, apply_command_token},
	{"ChoosePay", choose_pay, apply_pay},
	{"ChooseStrategy", choose_strategy, apply_strategy},
};
BSDATAF(choosestep)