#include "crt.h"
#include "card.h"
#include "entity.h"
#include "player.h"
#include "province.h"
#include "structure.h"
#include "troop.h"
#include "unit.h"

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

int	entity::getbonus(ability_s v) const {
	auto result = 0;
	if(bsdata<provincei>::have(this)) {
		for(auto& e : bsdata<structure>()) {
			if(e.getprovince() == this)
				result += e.get(v);
		}
	} else if(bsdata<playeri>::have(this)) {
		for(auto& e : bsdata<provincei>()) {
			if(e.player == this)
				result += e.get(v);
		}
		for(auto& e : bsdata<structure>()) {
			auto p = e.getprovince();
			if(p && p->player==this)
				result += e.get(v);
		}
		for(auto& e : bsdata<cardi>()) {
			if(e.location && e.player == this)
				result += e.get(v);
		}
	}
	return result;
}