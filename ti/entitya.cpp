#include "main.h"

bool find_entity(entity** ps, entity** pe, const entity* p) {
	while(ps < pe) {
		if(*ps == p)
			return true;
		ps++;
	}
	return false;
}

int entitya::fight(ability_s power, ability_s count) {
	auto result = 0;
	for(auto p : *this) {
		if(!p)
			continue;
		result += p->fight(p->get(power), p->get(count));
	}
	return result;
}

void entitya::select(const playeri* player, const entity* location) {
	auto ps = data;
	auto pe = endof();
	for(auto& e : bsdata<troop>()) {
		if(e.player != player || e.location != location)
			continue;
		if(ps < pe)
			*ps++ = &e;
	}
	count = ps - data;
}

void entitya::select(const playeri* player, const entity* system, unit_type_s type) {
	auto ps = data;
	auto pe = endof();
	for(auto& e : bsdata<troop>()) {
		if(e.player != player)
			continue;
		auto pu = e.getunit();
		if(!pu)
			continue;
		if(pu->type != type)
			continue;
		if(e.getsystem() != system)
			continue;
		if(ps < pe)
			*ps++ = &e;
	}
	count = ps - data;
}

void entitya::selectplanets(const systemi* system) {
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

void entitya::ingame() {
	auto ps = data;
	for(auto p : *this) {
		if(bsdata<systemi>::have(p)) {
			if(((systemi*)p)->index == pathfind::Blocked)
				continue;
		}
		*ps++ = p;
	}
	count = ps - data;
}

void entitya::select(array& source) {
	auto ps = data;
	auto px = endof();
	auto pe = source.end();
	auto size = source.getsize();
	for(auto pb = source.begin(); pb < pe; pb += size) {
		auto p = (entity*)pb;
		if(!(*p))
			continue;
		if(ps < px)
			*ps++ = p;
	}
	count = ps - data;
}

void entitya::match(const playeri* player, bool keep) {
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

void entitya::match(planet_trait_s value, bool keep) {
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

void entitya::match(color_s value, bool keep) {
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

void entitya::match(flag_s value, bool keep) {
	auto ps = data;
	for(auto p : *this) {
		if(!(*p))
			continue;
		if(p->is(value) != keep)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

void entitya::matchmove(int mode, bool keep) {
	auto ps = data;
	for(auto p : *this) {
		auto pd = p->getsystem();
		if(!pd)
			continue;
		switch(mode) {
		case 1:
			if(pd->movethrought() == keep)
				continue;
			break;
		default:
			if(pd->movestop() == keep)
				continue;
			break;
		}
		*ps++ = p;
	}
	count = ps - data;
}

void entitya::match(indicator_s value, bool keep) {
	auto ps = data;
	for(auto p : *this) {
		if(!(*p))
			continue;
		if(bsdata<planeti>::have(p)) {
			if((((planeti*)p)->get(value) > 0) != keep)
				continue;
		} else
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

void entitya::match(const systemi* system, bool keep) {
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

void entitya::grouplocation(const entitya& source) {
	auto ps = data;
	for(auto p : source) {
		if(!(*p))
			continue;
		if(!find_entity(data, ps, p->location))
			*ps++ = p->location;
	}
	count = ps - data;
}

void entitya::filter(const entity* object, bool keep) {
	auto ps = data;
	for(auto p : *this) {
		if((p == object) != keep)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

void entitya::activated(const playeri* player, bool keep) {
	auto ps = data;
	for(auto p : *this) {
		auto po = p->getsystem();
		if(po->isactivated(player) != keep)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}

entity* entitya::random() const {
	if(!count)
		return 0;
	return data[rand() % count];
}

static entity* choose_ai(const entitya& source, const char* id) {
	return source.random();
}

static entity* choose_human(const entitya& source, const char* id, const char* cancel) {
	answers an;
	for(auto p : source)
		an.add(p, p->getname());
	return (entity*)an.choose(getnm(id), cancel);
}

entity* entitya::choose(const char* id, const char* cancel) const {
	if(!count)
		return 0;
	if(!game.active->ishuman())
		return choose_ai(*this, id);
	else {
		auto value = data[0];
		if(bsdata<systemi>::have(value))
			return game.choosesystem(*this);
		return choose_human(*this, id, cancel);
	}
}

entity* entitya::getbest(indicator_s v) const {
	entity* result = 0;
	int result_value = 0;
	for(auto p : *this) {
		if(!result || result_value < p->get(v)) {
			result = p;
			result_value = p->get(v);
		}
	}
	return result;
}

void entitya::addu(entity* v) {
	if(!have(v))
		add(v);
}

void entitya::matchrange(int range, bool keep) {
	auto ps = data;
	for(auto p : *this) {
		auto po = p->getsystem();
		if((pathfind::getmove(po->index) <= range) != keep)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}