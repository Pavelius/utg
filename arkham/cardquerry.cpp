#include "main.h"

void cardquerry::add(cardpool& source, cardtype_s type) {
	auto pb = data + count;
	auto pe = endof();
	for(auto& e : source) {
		if(!e)
			continue;
		if(e.geti().type != type)
			continue;
		if(pb < pe)
			*pb++ = &e;
	}
	count = pb - data;
}

void cardquerry::add(cardpool& source, special_s type) {
	auto pb = data + count;
	auto pe = endof();
	for(auto& e : source) {
		if(!e)
			continue;
		if(e.geti().special != type)
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

bool cardquerry::isonly(tag_s v) const {
	if(!count)
		return false;
	for(auto p : *this) {
		if(*p && !p->geti().is(v))
			return false;
			
	}
	return true;
}