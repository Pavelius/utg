#include "action.h"

BSDATAC(actioni, 128)

const char* actioni::getprompt() const {
	auto pn = getdescription(id);
	if(!pn)
		pn = getnm(id);
	return pn;
}