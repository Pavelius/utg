#include "main.h"

bool find_entity(orderable** ps, orderable** pe, const orderable* p) {
	while(ps < pe) {
		if(*ps == p)
			return true;
		ps++;
	}
	return false;
}

int orderablea::fight(ability_s power, ability_s count) {
	auto result = 0;
	for(auto p : *this) {
		if(!p)
			continue;
		result += p->fight(p->get(power), p->get(count));
	}
	return result;
}

void orderablea::select(const playeri* player, const orderable* location) {
	auto ps = data;
	auto pe = endof();
	for(auto& e : bsdata<troop>()) {
		if(!e)
			continue;
		if(e.player != player || e.location != location)
			continue;
		if(ps < pe)
			*ps++ = &e;
	}
	count = ps - data;
}

void orderablea::selectplanets(const systemi* system) {
	auto ps = data;
	auto pe = endof();
	for(auto& e : bsdata<planeti>()) {
		if(!e)
			continue;
		if(e.location != system)
			continue;
		if(ps < pe)
			*ps++ = &e;
	}
	count = ps - data;
}

void orderablea::select(array& source) {
	auto ps = data;
	auto px = endof();
	auto pe = source.end();
	auto size = source.getsize();
	for(auto pb = source.begin(); pb < pe; pb += size) {
		auto p = (orderable*)pb;
		if(!(*p))
			continue;
		if(ps < px)
			*ps++ = p;
	}
	count = ps - data;
}

void orderablea::match(const playeri* player, bool keep) {
	auto ps = data;
	for(auto p : *this) {
		if(!(*p))
			continue;
		if((p->player == player) != keep)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

void orderablea::match(planet_trait_s value, bool keep) {
	auto ps = data;
	for(auto p : *this) {
		if(!(*p))
			continue;
		if((p->gettrait() == value) != keep)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

void orderablea::match(color_s value, bool keep) {
	auto ps = data;
	for(auto p : *this) {
		if(!(*p))
			continue;
		if((p->getspeciality() == value) != keep)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

void orderablea::match(const systemi* system, bool keep) {
	auto ps = data;
	for(auto p : *this) {
		if(!(*p))
			continue;
		if((p->getsystem() == system) != keep)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

void orderablea::selectlocation(const orderablea& source) {
	auto ps = data;
	for(auto p : source) {
		if(!(*p))
			continue;
		if(!find_entity(data, ps, p->location))
			*ps++ = p;
	}
	count = ps - data;
}

void orderablea::filter(const orderable* object, bool keep) {
	auto ps = data;
	for(auto p : *this) {
		if((p==object)!=keep)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

orderable* orderablea::choose(const char* id) const {
	return 0;
}