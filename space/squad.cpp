#include "main.h"

int squad::get(effect_s v) const {
	switch(v) {
	case Injury: return injury;
	case Exhause: return exhause;
	case Gear: return gear;
	case Morale: return morale;
	case Armor: return armor + getmaximum(v);
	default: return getmaximum(v);
	}
}

int squad::getmaximum(effect_s v) const {
	int r = 0;
	auto m = bsdata<tagi>::source.getcount();
	for(auto tag = (tag_s)0; tag < m; tag = (tag_s)(tag + 1)) {
		if(!is(tag))
			continue;
		if(!bsdata<tagi>::get(tag).is(v))
			continue;
		r++;
	}
	return r;
}

void squad::set(effect_s v, int i) {
	switch(v) {
	case Injury: injury = i; break;
	case Exhause: exhause = i; break;
	case Gear: gear = i; break;
	case Morale: morale = i; break;
	default: break;
	}
}

bool squad::is(condition_s v) const {
	switch(v) {
	case GoodState: return injury == getmaximum(Injury);
	case MiddleState: return injury && injury < getmaximum(Injury);
	case BadState: return !injury;
	}
	return false;
}

void squad::getinfo(stringbuilder& sb) const {
}

bool squad::isallow(move_s v) const {
	switch(v) {
	case EngageMelee:
		return distance == Close && is(Close);
	case TargetSomeone:
		return distance == Far && is(Far);
	default:
		return true;
	}
}