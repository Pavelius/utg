#include "main.h"

static indext getid(const treasurei* pv) {
	auto i = bsdata<treasurei>::source.indexof(pv);
	if(i == -1)
		return 0xFFFF;
	return i;
}

static const treasurei* getobject(indext v) {
	if(v == 0xFFFF)
		return 0;
	return bsdata<treasurei>::elements + v;
}

void chest::gaintreasure(const treasurei* p) {
	auto i = getid(p);
	if(i == 0xFFFF)
		return;
	p->gaining();
	add(i);
	game.information(getnm("GainTreasure"), getnm(p->id));
}

void chest::losstreasure(const treasurei* p) {
	auto i = getid(p);
	if(i == 0xFFFF)
		return;
	auto need_lossing = false;
	auto pe = end();
	auto ps = data;
	for(auto pb = data; pb < pe; pb++) {
		if(*pb == i) {
			game.warning(getnm("LoseTreasure"), getnm(p->id));
			need_lossing = true;
			continue;
		}
		*ps++ = *pb;
	}
	count = ps - data;
	if(need_lossing)
		p->lossing();
}

int chest::getbonus(ability_s v) const {
	auto r = 0;
	for(auto p : gettreasures())
		r += p->abilities[v - Exploration];
	return r;
}

int chest::gettreasurecount(tag_s v) const {
	auto r = 0;
	for(auto p : gettreasures()) {
		if(p->is(v))
			r++;
	}
	return r;
}

const treasurei* chest::choosetreasure(const char* title, const char* cancel) const {
	answers an;
	for(auto p : gettreasures())
		an.add(p, getnm(p->id));
	return (treasurei*)an.choose(title, cancel);
}

void chest::apply(trigger_s type, ability_s v) {
	for(auto p : gettreasures()) {
		if(p->trigger != type)
			continue;
		if(p->ability != v)
			continue;
		p->apply();
	}
}