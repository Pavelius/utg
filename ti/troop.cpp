#include "main.h"

troop* troop::last;

static troop* addnew() {
	for(auto& e : bsdata<troop>()) {
		if(!e.type)
			return &e;
	}
	return bsdata<troop>::add();
}

troop* troop::add(const char* id, playeri* player) {
	auto pu = bsdata<uniti>::find(id);
	if(!pu)
		return 0;
	return add(pu, player, 0);
}

troop* troop::add(const uniti* unit, playeri* player, entity* location) {
	auto p = addnew();
	p->clear();
	p->type = unit;
	p->player = player;
	p->location = location;
	return p;
}

int	troop::getstackcount() const {
	auto pe = bsdata<troop>::end();
	auto result = 1;
	auto v1 = location;
	auto v2 = type;
	for(auto p = this + 1; p < pe; p++) {
		if(p->location == v1 && p->type==v2)
			result++;
	}
	return result;
}

troop* troop::getstackholder() {
	auto pb = bsdata<troop>::begin();
	auto result = this;
	auto v1 = location;
	auto v2 = type;
	for(auto p = this - 1; p >= pb; p--) {
		if(p->location == v1 && p->type == v2)
			result = p;
	}
	return result;
}

static const char* upload_name = "Upload";

void troop::movement(entity* destination) {
	answers an;
	auto cmd_upload = bsdata<choosestep>::find("Upload");
	while(true) {
		an.clear();
		if(cmd_upload && get(Capacity))
			an.add(cmd_upload, getnm(cmd_upload->id));
		auto result = an.choose(getname(), getnm("Movement"), 1);
		if(!result) {
			location = destination;
			game.updateui();
			break;
		} else if(result == upload_name)
			upload();
	}
}

void troop::upload() {
	while(true) {
		entitya querry;
		querry.select(player, location, GroundForces);
		auto result = querry.choose(getname(), getnm("EndLoad"));
		if(!result)
			break;
		auto p = (troop*)result;
		p->location = this;
		game.updateui();
	}
}

void troop::produce(const uniti* unit) const {
	auto last_system = systemi::last;
	auto last_planet = planeti::last;
	systemi::last = getsystem();
	planeti::last = getplanet();
	unit->placement(unit->abilities[CostCount], false);
	planeti::last = last_planet;
	systemi::last = last_system;
}

void troop::add(answers& an) {
	auto planet = getplanet();
	if(planet)
		an.add(this, "%1 (%2)", getname(), planet->getname());
	else
		an.add(this, getname());
}