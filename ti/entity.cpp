#include "answers.h"
#include "card.h"
#include "deck.h"
#include "entity.h"
#include "player.h"
#include "planet.h"
#include "planet_trait.h"
#include "system.h"
#include "troop.h"
#include "unit.h"

int entity::get(ability_s v) const {
	if(bsdata<troop>::have(this))
		return ((uniti*)id)->abilities[v];
	else if(bsdata<uniti>::have(this) || bsdata<prototype>::have(this))
		return ((uniti*)this)->abilities[v];
	return 0;
}

bool entity::is(tag_s v) const {
	if(v <= Exhaust)
		return (flags & (1 << v)) != 0;
	if(bsdata<uniti>::have(this) || bsdata<prototype>::have(this))
		return ((uniti*)this)->tags.is(v);
	return false;
}

void entity::set(tag_s v, bool n) {
	if(v <= Exhaust) {
		if(n)
			flags |= 1 << v;
		else
			flags &= ~(1 << v);
	} else if(bsdata<uniti>::have(this) || bsdata<prototype>::have(this))
		((uniti*)this)->tags.set(v, n);
}

int	entity::getsumary(unit_type_s v) const {
	auto result = 0;
	for(auto& e : bsdata<troop>()) {
		if(e.location != this)
			continue;
		if(e.getunit()->type==v)
			result++;
	}
	return result;
}

int	entity::getsumary(ability_s v) const {
	auto result = 0;
	for(auto& e : bsdata<troop>()) {
		if(e.location != this)
			continue;
		result += e.get(v);
	}
	return result;
}

const char* entity::getname() const {
	return getnm(getid());
}

const char* entity::getid() const {
	if(bsdata<uniti>::have(id) || bsdata<prototype>::have(id))
		return ((uniti*)id)->id;
	else if(bsdata<componenti>::have(id))
		return ((componenti*)id)->id;
	else if(id)
		return id;
	return "";
}

systemi* entity::getsystem() const {
	if(!this)
		return 0;
	if(bsdata<troop>::have(this))
		return ((troop*)this)->location->getsystem();
	else if(bsdata<planeti>::have(this))
		return ((planeti*)this)->location->getsystem();
	else if(bsdata<systemi>::have(this))
		return (systemi*)this;
	return 0;
}

planeti* entity::getplanet() const {
	if(!this)
		return 0;
	if(bsdata<troop>::have(this))
		return ((troop*)this)->location->getplanet();
	else if(bsdata<planeti>::have(this))
		return (planeti*)this;
	return 0;
}

planet_trait_s entity::gettrait() const {
	if(bsdata<planeti>::have(this))
		return ((planeti*)this)->trait;
	return NoTrait;
}

color_s entity::getspeciality() const {
	if(bsdata<planeti>::have(this))
		return ((planeti*)this)->speciality;
	return NoTech;
}

int	entity::get(indicator_s v) const {
	if(bsdata<planeti>::have(this))
		return ((planeti*)this)->get(v);
	return 0;
}

void entity::hit() {
	if(bsdata<troop>::have(this))
		((troop*)this)->hit();
}

const uniti* entity::getunit() const {
	if(bsdata<uniti>::have(id) || bsdata<prototype>::have(id))
		return (uniti*)id;
	return 0;
}

const componenti* entity::getcard() const {
	if(bsdata<componenti>::have(id))
		return (componenti*)id;
	return 0;
}

decki* entity::getdeck() const {
	if(bsdata<card>::have(this))
		return (decki*)((componenti*)id)->location;
	else if(bsdata<componenti>::have(this))
		return (decki*)location;
	return 0;
}

int	entity::getproduction() const {
	auto result = 0;
	auto pu = getunit();
	if(pu) {
		result += pu->abilities[Production];
		if(pu->tags.is(AddPlanetResourceToProduction)) {
			auto planet = getplanet();
			if(planet)
				result += planet->get(Resources);
		}
	}
	return result;
}

playeri* entity::getenemy() const {
	for(auto& e : bsdata<troop>()) {
		if(e.location == this && e.player != player)
			return e.player;
	}
	return 0;
}

void entity::status(int type, const char* format) const {
	auto isgood = (type >= 0);
	if(!player->ishuman())
		isgood = !isgood;
	if(isgood)
		information(format, getname());
	else
		warning(format, getname());
}

void entity::exhaust() {
	if(!this)
		return;
	if(is(Exhaust))
		return;
	set(Exhaust, true);
}

void entity::discard() {
	auto p = getdeck();
	if(p)
		p->cards.add(this); // To the down of deck
	flags = 0; // Remove all players token
	player = 0; // Clear player
}

playeri* entity::getplayer() const {
	return player;
}