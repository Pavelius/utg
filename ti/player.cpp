#include "main.h"

playeri* playeri::last;
playeri* playeri::human;

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

void playeri::setcontrol(planeti* planet) {
	if(planet->player != this) {
		planet->player = this;
		draw::information(getnm("SetControlPlanet"),
			getnm(id), getnm(planet->id));
	}
}

systemi* playeri::gethome() const {
	for(auto& e : bsdata<systemi>()) {
		if(e.home == this)
			return &e;
	}
	return 0;
}