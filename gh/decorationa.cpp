#include "main.h"

void decorationa::select() {
	auto ps = data;
	auto pe = endof();
	for(auto& e : bsdata<decoration>()) {
		if(!e)
			continue;
		if(ps < pe)
			*ps++ = &e;
	}
	count = ps - data;
}

void decorationa::range(int v) {
	auto ps = data;
	auto pe = endof();
	for(auto p : *this) {
		if(!(*p))
			continue;
		if(pathfind::getmove(p->getindex()) > v)
			continue;
		if(ps < pe)
			*ps++ = p;
	}
	count = ps - data;
}