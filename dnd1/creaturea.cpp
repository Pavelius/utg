#include "main.h"

void creaturea::select() {
	auto ps = begin();
	auto pe = endof();
	for(auto& e : bsdata<creature>()) {
		if(ps < pe)
			*ps++ = &e;
	}
	count = ps - data;
}

void creaturea::match(feat_s v, bool keep) {
	auto ps = begin();
	for(auto p : *this) {
		if(p->is(v) != keep)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

void creaturea::matchenemy(bool keep) {
	auto ps = begin();
	for(auto p : *this) {
		if((p->enemy_index != 0xFF) != keep)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

void creaturea::matchready(bool keep) {
	auto ps = begin();
	for(auto p : *this) {
		if(p->isready()!=keep)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

creature* creaturea::choose(const char* title) const {
	answers an;
	for(auto p : *this)
		an.add(p, getnm(p->getname()));
	return (creature*)an.choose(title, 0, 2);
}