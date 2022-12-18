#include "main.h"

void strategyi::getinfo(stringbuilder& sb) const {
	auto pn = getdescription(id);
	if(!pn)
		return;
	sb.addn("---");
	sb.addn(pn);
}

static void addreq(stringbuilder& sb, const char* requiqment) {
	auto pn = sb.get();
	for(auto i = Red; i <= Yellow; i = (color_s)(i + 1)) {
		if(!requiqment[i])
			continue;
		if(pn[0])
			sb.add(",");
		else {
			sb.addn("%Required:");
		}
		sb.adds("%1i %2", requiqment[i], bsdata<colori>::elements[i].getname());
	}
}

void techi::getinfo(stringbuilder& sb) const {
	sb.addn("---");
	sb.addn("%1 %-Tech", bsdata<colori>::elements[color].getname());
	addreq(sb, required);
	auto pn = getdescription(id);
	if(pn) {
		sb.addn("---");
		sb.addn(pn);
	}
}