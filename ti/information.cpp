#include "main.h"

void strategyi::getinfo(stringbuilder& sb) const {
	auto pn = getdescription(id);
	if(!pn)
		return;
	sb.addn("---");
	sb.addn(pn);
}