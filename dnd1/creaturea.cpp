#include "main.h"

void creaturea::select() {
	auto ps = begin();
	auto pe = endof();
	for(auto& e : bsdata<creature>()) {
		if(ps<pe)
			*ps++ = &e;
	}
	count = ps - data;
}

void creaturea::match(feat_s v, bool keep) {
	auto ps = begin();
	for(auto& e : bsdata<creature>()) {
		if(e.is(v)!=keep)
			continue;
		*ps++ = &e;
	}
	count = ps - data;
}