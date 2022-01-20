#include "main.h"

int statable::get(effect_s v) const {
	int r;
	switch(v) {
	case Injury: return injury;
	case Exhause: return exhause;
	case Gear: return gear;
	case Morale: return morale;
	case Armor:
		r = 0;
		if(is(Armored))
			r++;
		if(is(Vehicle))
			r++;
		return r;
	default: return 0;
	}
}

int statable::getmaximum(effect_s v) const {
	int r;
	switch(v) {
	case Injury:
		r = 2;
		if(is(Regular))
			r++;
		if(is(Veteran))
			r++;
		if(is(Elite))
			r++;
		return r;
	case Exhause:
		r = 1;
		if(is(Regular))
			r++;
		if(is(Elite))
			r++;
		return r;
	case Gear:
		r = 0;
		if(is(HeavyWeapon))
			r++;
		if(is(Veteran))
			r++;
		return gear;
	case Morale:
		r = 1;
		if(is(Elite))
			r++;
		return r;
	default:
		return 0;
	}
}

void statable::set(effect_s v, int i) {
	switch(v) {
	case Injury: injury = i; break;
	case Exhause: exhause = i; break;
	case Gear: gear = i; break;
	case Morale: morale = i; break;
	default: break;
	}
}