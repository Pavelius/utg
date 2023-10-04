#include "crt.h"
#include "entity.h"
#include "player.h"
#include "troop.h"
#include "unit.h"

bool entity::is(const playeri* v) const {
	if(!v)
		return false;
	return (tags & ((0x80000000) >> getbsi(v))) != 0;
}

void entity::set(const playeri* v) {
	if(!v)
		return;
	tags |= ((0x80000000) >> getbsi(v));
}

int entity::get(ability_s v) const {
	if(bsdata<troopi>::have(this))
		return ((uniti*)id)->abilities[v];
	else if(bsdata<playeri>::have(this))
		return ((playeri*)this)->current.abilities[v];
	return 0;
}

uniti* entity::getunit() const {
	if(bsdata<troopi>::have(this))
		return ((uniti*)id);
	return 0;
}