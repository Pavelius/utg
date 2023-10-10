#include "crt.h"
#include "card.h"
#include "draw_object.h"
#include "entity.h"
#include "player.h"
#include "province.h"
#include "structure.h"
#include "troop.h"
#include "unit.h"

void entity::clear() {
	auto p = findobject(this);
	if(p)
		p->clear();
	memset(this, 0, sizeof(*this));
}

const char*	entity::getname() const {
	if(bsdata<troopi>::have(this))
		return getunit()->getname();
	else if(bsdata<card>::have(this))
		return getcomponent()->getname();
	return nameable::getname();
}

bool entity::is(const entity* v) const {
	if(!v)
		return false;
	if(bsdata<playeri>::have(v))
		return (tags & ((0x80000000) >> getbsi((playeri*)v))) != 0;
	return false;
}

void entity::set(const entity* v) {
	if(!v)
		return;
	if(bsdata<playeri>::have(v))
		tags |= ((0x80000000) >> getbsi((playeri*)v));
}

void entity::remove(const entity* v) {
	if(!v)
		return;
	if(bsdata<playeri>::have(v))
		tags &= ~((0x80000000) >> getbsi((playeri*)v));
}

int entity::get(ability_s v) const {
	if(bsdata<troopi>::have(this))
		return ((uniti*)id)->abilities[v];
	else if(bsdata<structure>::have(this))
		return ((structurei*)id)->abilities[v];
	else if(bsdata<playeri>::have(this))
		return ((playeri*)this)->current.abilities[v];
	else if(bsdata<uniti>::have(this))
		return ((uniti*)this)->get(v);
	else if(bsdata<provincei>::have(this))
		return ((provincei*)this)->abilities[v];
	else if(bsdata<structurei>::have(this))
		return ((structurei*)this)->abilities[v];
	return 0;
}

uniti* entity::getunit() const {
	if(bsdata<troopi>::have(this))
		return ((uniti*)id);
	return 0;
}

provincei* entity::getprovince() const {
	if(bsdata<provincei>::have(this))
		return static_cast<provincei*>(const_cast<entity*>(this));
	else if(bsdata<provincei>::have(location))
		return static_cast<provincei*>(const_cast<entity*>(location));
	return 0;
}

playeri* entity::getplayer() const {
	if(bsdata<playeri>::have(this))
		return static_cast<playeri*>(const_cast<entity*>(this));
	return player;
}

bool entity::ishomeland() const {
	auto p = getprovince();
	if(!p)
		return p;
	for(auto& e : bsdata<playeri>()) {
		if(e.homeland == p)
			return true;
	}
	return false;
}

int	entity::getbonus(ability_s v) const {
	auto result = 0;
	if(bsdata<provincei>::have(this)) {
		for(auto& e : bsdata<structure>()) {
			if(e.location == this)
				result += e.get(v);
		}
		for(auto& e : bsdata<card>()) {
			if(e.location == this)
				result += e.get(v);
		}
	} else if(bsdata<playeri>::have(this)) {
		auto p = (playeri*)this;
		for(auto& e : bsdata<provincei>()) {
			if(e.player == this)
				result += e.abilities[v];
		}
		for(auto& e : bsdata<structure>()) {
			auto p = e.getprovince();
			if(p && p->player == this)
				result += e.get(v);
		}
		for(auto& e : bsdata<card>()) {
			if(e.player == this || (e.location && e.location->player==this))
				result += e.get(v);
		}
		for(auto& e : bsdata<upgradei>()) {
			if(p->isupgrade(&e))
				result += e.abilities[v];
		}
	}
	return result;
}

cardi* entity::getcomponent() const {
	if(bsdata<card>::have(this))
		return (cardi*)id;
	return 0;
}

void entity::pick(playeri* v) {
	if(bsdata<card>::have(this)) {
		player = v;
		location = 0;
	}
}