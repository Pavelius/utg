#include "main.h"

void cardquerry::add(cardpool& source, cardtype_s type) {
	auto pb = data;
	auto pe = endof();
	for(auto& e : source.source) {
		if(!e)
			continue;
		if(e.type != type)
			continue;
		if(pb < pe)
			*pb++ = &e;
	}
	count = pb - data;
}

int	cardquerry::get(ability_s v) const {
	auto r = 0;
	for(auto p : *this)
		r += p->geti().abilities[v];
	return r;
}