#include "main.h"

indext chest::getid(const treasurei* pv) {
	auto i = bsdata<treasurei>::source.indexof(pv);
	if(i == -1)
		return 0xFFFF;
	return i;
}

const treasurei* chest::getobject(indext v) {
	if(v == 0xFFFF)
		return 0;
	return bsdata<treasurei>::elements + v;
}

void chest::gaintreasure(const treasurei* p) {
	auto i = getid(p);
	if(i == 0xFFFF)
		return;
	add(i);
	game.information(getnm("GainTreasure"), getnm(p->id));
	p->gaining();
}

void chest::losstreasure(const treasurei* p) {
	auto i = getid(p);
	if(i == 0xFFFF)
		return;
	auto pe = end();
	auto ps = data;
	for(auto pb = data; pb < pe; pb++) {
		if(*pb == i) {
			p->lossing();
			continue;
		}
		*ps++ = *pb;
	}
	count = ps - data;
}

int chest::getbonus(ability_s v) const {
	auto r = 0;
	for(auto pi : *this) {
		auto p = getobject(pi);
		if(!p)
			continue;
		r += p->abilities[v - Exploration];
	}
	return r;
}

const treasurei* chest::choosetreasure(const char* title, const char* cancel) const {
	answers an;
	for(auto v : *this) {
		auto p = getobject(v);
		if(!p)
			continue;
		an.add(p, getnm(p->id));
	}
	//if(allow_stopbury)
	//	an.add(0, getnm("StopBury"));
	return (treasurei*)an.choose(title, cancel, utg::interactive, utg::url, -1, utg::header, utg::sb.begin());
}