#include "main.h"

playeri* playeri::last;

static indicator_s command_tokens[] = {TacticToken, FleetToken, StrategyToken};

static void add_token(answers& an) {
	for(auto v : command_tokens) {
		auto& e = bsdata<indicatori>::elements[v];
		an.add(&e, getnm(e.id));
	}
}

static bool apply_indicator() {
	if(bsdata<indicatori>::have(game.result)) {
		auto v = (indicator_s)bsdata<indicatori>::source.indexof(game.result);
		playeri::last->add(v, 1);
		game.options--;
	} else
		return false;
	return true;
}

void playeri::add(indicator_s v, int i) {
	auto n0 = get(v);
	auto n1 = n0 + i;
	if(n0 == n1)
		return;
	if(i > 0)
		draw::information(getnm("RaiseIndicatorTo"),
			getnm(id), getnm(bsdata<indicatori>::elements[v].id), n1);
	indicators[v] = n1;
}

void playeri::addcommand(int count) {
	game.options = count;
	game.choose(WhenPay, "AddCommandToken", add_token, apply_indicator);
}

void playeri::setcontrol(planeti* planet) {
	if(planet->player != this) {
		planet->player = this;
		draw::information(getnm("SetControlPlanet"),
			getnm(id), getnm(planet->id));
	}
}

static void select_planets(const playeri* player, bool iscontrol) {
	querry.clear();
	querry.select(bsdata<planeti>::source);
	querry.match(player, iscontrol);
}

static indicator_s last_indicator;

static void add_planets(answers& an) {
	select_planets(playeri::last, true);
	querry.match(Exhaust, false);
	querry.match(last_indicator, true);
	for(auto p : querry)
		an.add(p, getnm("PayAnswer"), getnm(p->id), p->get(last_indicator));
}

static bool apply_pay() {
	if(bsdata<planeti>::have(game.result)) {
		auto p = (planeti*)game.result;
		game.options -= p->get(last_indicator);
		p->exhaust();
	} else
		return false;
	return true;
}

void playeri::pay(indicator_s type, int count) {
	last_indicator = type;
	game.options = count;
	game.choose(WhenPay, "PayPrompt", add_planets, apply_pay);
}