#include "main.h"

playeri* playeri::active;
playeri* playeri::last;

static indicator_s command_tokens[] = {TacticToken, FleetToken, StrategyToken};

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
	draw::output(getnm("AddCommandToken"), count);
	answers an;
	while(count > 0) {
		an.clear();
		for(auto v : command_tokens) {
			auto& e = bsdata<indicatori>::elements[v];
			an.add(&e, getnm(e.id));
		}
		auto pe = an.choose();
		if(!pe)
			break;
		if(bsdata<indicatori>::have(pe)) {
			auto v = (indicator_s)bsdata<indicatori>::source.indexof(pe);
			last->add(v, 1);
			count--;
		} else
			break;
	}
}

void playeri::setcontrol(planeti* planet) {
	if(planet->player != this) {
		planet->player = this;
		draw::information(getnm("SetControlPlanet"),
			getnm(id), getnm(planet->id));
	}
}