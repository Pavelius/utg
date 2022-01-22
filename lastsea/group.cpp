#include "main.h"

const groupi* groupi::find(const char* id) {
	for(auto& e : bsdata<groupi>()) {
		if(strcmp(e.id, id) == 0)
			return &e;
	}
	return 0;
}

variant groupi::choose(short unsigned parent) {
	answers an;
	for(auto& e : bsdata<valuei>()) {
		if(e.type == parent)
			an.add(&e, getnm(e.id));
	}
	char temp[260]; stringbuilder sb(temp);
	sb.add(getnm(bsdata<groupi>::elements[parent].id));
	return logs::choose(an, temp, 0);
}