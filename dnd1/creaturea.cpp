#include "answers.h"
#include "creature.h"
#include "rand.h"

creaturea creatures, encountered, targets;

void creaturea::select() {
	auto ps = begin();
	auto pe = endof();
	for(auto& e : bsdata<creature>()) {
		if(ps < pe)
			*ps++ = &e;
	}
	count = ps - data;
}

void creaturea::set(featn v) {
	for(auto p : *this)
		p->set(v);
}

void creaturea::match(featn v, bool keep) {
	auto ps = begin();
	for(auto p : *this) {
		if(p->is(v) != keep)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

void creaturea::matchenemy(bool keep) {
	if(player->is(Enemy))
		match(Player, keep);
	else
		match(Enemy, keep);
}

void creaturea::matchally(bool keep) {
	if(player->is(Enemy))
		match(Enemy, keep);
	else
		match(Player, keep);
}

void creaturea::match(creaturea::fnproc proc, bool keep) {
	auto ps = begin();
	for(auto p : *this) {
		if((p->*proc)() != keep)
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

creature* creaturea::random() const {
	return count ? data[rand() % count] : 0;
}