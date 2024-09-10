#include "entity.h"
#include "player.h"
#include "province.h"
#include "site.h"

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
	if(bsdata<sitei>::have(this))
		return ((sitei*)this)->upkeep;
	return 0;
}

const short* entityi::geteffect() const {
	if(bsdata<playeri>::have(this))
		return ((landscapei*)this)->income;
	else if(bsdata<sitei>::have(this))
		return ((sitei*)this)->income;
	else if(bsdata<provincei>::have(this))
		return ((provincei*)this)->income;
	return 0;
}