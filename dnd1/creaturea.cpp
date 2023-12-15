#include "answers.h"
#include "creature.h"

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
		if((p->enemy != 0) != keep)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

void creaturea::matchready(bool keep) {
	auto ps = begin();
	for(auto p : *this) {
		if(p->isready() != keep)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

creature* creaturea::choose(const char* title, bool random) const {
	if(!count)
		return 0;
	if(random)
		return data[rand() % count];
	answers an;
	for(auto p : *this)
		an.add(p, p->getname());
	return (creature*)an.choose(title, 0, 2);
}