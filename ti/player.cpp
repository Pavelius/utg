#include "main.h"

playeri* playeri::last;
playeri* playeri::human;

int	playeri::getindex() const {
	return game.origin_players.find(const_cast<playeri*>(this));
}

void playeri::act(const char* format, const char* value) const {
	if(!answers::console)
		return;
	answers::console->addn("[+");
	answers::console->add(getnm(format), getnm(id), value);
	answers::console->add("]");
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

bool playeri::canbuild(const uniti* unit) const {
	auto maxcount = unit->abilities[Reinforcement];
	if(maxcount < 50) {
		auto count = getsummary(unit);
		if(count >= maxcount)
			return false;
	}
	return true;
}

int	playeri::getsummary(const uniti* type) const {
	auto result = 0;
	for(auto& e : bsdata<troop>()) {
		if(e.player == this && e.getunit() == type)
			result++;
	}
	return result;
}

int	playeri::getplanetsummary(indicator_s v) const {
	entitya source;
	source.select(bsdata<planeti>::source);
	source.match(this, true);
	source.match(Exhaust, false);
	return source.getsummary(v);
}

int	playeri::getinitiative() const {
	if(!strategy)
		return 0;
	return strategy->initiative;
}

int playeri::getcards() const {
	entitya source;
	source.selectcards(this);
	return source.getcount();
}

int playeri::gettechs() const {
	auto result = 0;
	for(auto i = PlasmaScoring; i <= IntegratedEconomy; i = (tech_s)(i + 1)) {
		if(is(i))
			result++;
	}
	return result;
}

void playeri::assign(variants source) {
	for(auto v : source) {
		if(v.iskind<techi>())
			tech.set(v.value);
	}
}

const uniti* playeri::getunit(int index) const {
	if(index == -1)
		return 0;
	if(index < (int)(sizeof(prototype::units) / sizeof(prototype::units[0]))) {
		auto i = getindex();
		if(i == -1)
			return 0;
		return bsdata<prototype>::elements[i].units + index;
	}
	return bsdata<uniti>::elements + index;
}

void playeri::getadvance(requirement& result) const {
	for(auto i = PlasmaScoring; i <= IntegratedEconomy; i = (tech_s)(i + 1)) {
		if(!is(i))
			continue;
		result.required[bsdata<techi>::elements[i].color - 1]++;
	}
}