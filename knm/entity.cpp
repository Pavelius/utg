#include "crt.h"
#include "entity.h"
#include "player.h"
#include "province.h"
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
	else if(bsdata<playeri>::have(this))
		return ((playeri*)this)->current.abilities[v];
	else if(bsdata<uniti>::have(this))
		return ((uniti*)this)->get(v);
	else if(bsdata<provincei>::have(this))
		return ((provincei*)this)->abilities[v];
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