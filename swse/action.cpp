#include "action.h"

BSDATAC(actioni, 128)

actioni* last_action;

const char* actioni::getprompt() const {
	auto pn = getnme(ids(id, "Ask"));
	if(!pn)
		pn = getnm(id);
	return pn;
}