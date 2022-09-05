#include "main.h"

void itema::select(creature& source) {
	for(auto& e : source.wears) {
		if(e)
			add(&e);
	}
}

void itema::match(wear_s wear, bool keep) {
	auto ps = begin();
	for(auto p : *this) {
		if((p->geti().wear==wear) != keep)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}