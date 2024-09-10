#include "building.h"
#include "entity.h"
#include "player.h"
#include "province.h"

const landscapei* entityi::getlandscape() const {
	if(bsdata<landscapei>::have(this))
		return (landscapei*)this;
	auto p = getprovince();
	if(p)
		return p->landscape;
	return 0;
}

const cardi* entityi::getcard() const {
	if(bsdata<cardi>::have(this))
		return (cardi*)this;
	return 0;
}

const short* entityi::getupkeep() const {
	if(bsdata<landscapei>::have(this))
		return ((landscapei*)this)->upkeep;
	else if(bsdata<buildingi>::have(this))
		return ((buildingi*)this)->upkeep;
	return 0;
}

const short* entityi::geteffect() const {
	if(bsdata<playeri>::have(this))
		return ((landscapei*)this)->effect;
	else if(bsdata<buildingi>::have(this))
		return ((buildingi*)this)->effect;
	else if(bsdata<provincei>::have(this))
		return ((provincei*)this)->effect;
	return 0;
}