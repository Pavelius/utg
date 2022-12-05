#include "ability.h"
#include "damage.h"
#include "item.h"
#include "list.h"
#include "modifier.h"
#include "main.h"
#include "skill.h"

bool statable::addstart(variant v, modifier_s modifier, bool run) {
	if(v.iskind<abilityi>()) {
		if(modifier == Proficient) {
			if(v.value >= saves.getmaximum())
				return false;
			if(saves.is(v.value))
				return false;
			if(run)
				saves.set(v.value);
		} else {
			if(run)
				abilitites[v.value] += v.counter;
		}
	} else if(v.iskind<itemi>()) {
		if(items.is(v.value))
			return false;
		if(run)
			items.set(v.value);
	} else if(v.iskind<damagei>()) {
		switch(modifier) {
		case Resist:
			if(resist.is(v.value))
				return false;
			if(run)
				resist.set(v.value);
			break;
		case Immunity: immunity.set(v.value); break;
		case Vulnerable: vulnerable.set(v.value); break;
		default: return false;
		}
	} else if(v.iskind<skilli>()) {
		if(skills.is(v.value))
			return false;
		if(run)
			skills.set(v.value);
	} else if(v.iskind<listi>()) {
		for(auto v : bsdata<listi>::elements[v.value].elements) {
			if(!addstart(v, modifier, run))
				return false;
		}
	}
	return true;
}