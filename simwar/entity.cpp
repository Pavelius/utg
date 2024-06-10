#include "crt.h"
#include "entity.h"
#include "province.h"

const landscapei* entityi::getlandscape() const {
	if(bsdata<landscapei>::have(this))
		return (landscapei*)this;
	auto p = getprovince();
	if(p)
		return p->landscape;
	return 0;
}