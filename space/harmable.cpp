#include "main.h"

static effect_s squad_param[] = {Injury, Exhause, Morale, Gear};

void harmable::clear() {
	memset(this, 0, sizeof(*this));
}

int harmable::get(effect_s v) const {
	switch(v) {
	case Injury: return injury;
	case Exhause: return exhause;
	case Gear: return gear;
	case Morale: return morale;
	default: return getmaximum(v);
	}
}

void harmable::set(effect_s v, int i) {
	switch(v) {
	case Injury: injury = i; break;
	case Exhause: exhause = i; break;
	case Gear: gear = i; break;
	case Morale: morale = i; break;
	default: break;
	}
}

int harmable::getmaximum(effect_s v) const {
	int r = 0;
	switch(v) {
	case Injury:
		r = 1;
		if(is(Regular))
			r++;
		if(is(Veteran))
			r++;
		if(is(Elite))
			r++;
		if(is(Tought))
			r++;
		break;
	case Gear:
		if(is(Armored))
			r++;
		if(is(Veteran) || is(Elite))
			r++;
		if(is(HeavyWeapon) || is(Explosion))
			r++;
		break;
	case Morale:
		r = 1;
		if(is(Veteran) || is(Elite))
			r++;
		if(is(Leader))
			r += 2;
		break;
	case Exhause:
		r = 1;
		if(is(Elite))
			r++;
		if(is(Agile))
			r++;
		break;
	default:
		break;
	}
	return r;
}

void harmable::getharm(harmable& result) const {
	result.injury = 1;
	if(is(Regular))
		result.injury++;
	if(is(Veteran) || is(Elite))
		result.injury++;
	if(is(Rifle))
		result.injury++;
	if(is(Vehicle))
		result.injury++;
	if(is(Tricky))
		result.exhause++;
	if(is(Smashing))
		result.gear++;
}

int	harmable::getarmor() const {
	int r = 0;
	if(is(Armored))
		r++;
	if(is(Vehicle))
		r++;
	return r;
}

bool harmable::is(condition_s v) const {
	switch(v) {
	case GoodState: return injury == getmaximum(Injury);
	case MiddleState: return injury && injury < getmaximum(Injury);
	case BadState: return !injury;
	}
	return false;
}

int harmable::getdistinct() const {
	auto n = 0;
	for(auto v : squad_param) {
		if(get(v))
			n++;
	}
	return n;
}

void harmable::getinfo(stringbuilder& sb) {
	auto n = getdistinct();
	auto pb = sb.get(); pb[0] = 0;
	for(auto vi : squad_param) {
		auto v = get(vi);
		if(v <= 0)
			continue;
		auto pn = bsdata<effecti>::elements[vi].id;
		if(pb[0]) {
			if(n == 1)
				sb.adds("%-And");
			else
				sb.add(",");
		}
		if(v == 1)
			sb.adds("%-1", getnm(pn));
		else
			sb.adds("[%2i] %1", sb.getbycount(pn, v), v);
		n--;
	}
}