#include "main.h"

modifier_s modifieri::last;

void moveable::apply(variant v) {
	if(v.iskind<abilityi>()) {
		if(modifieri::last == Proficient) {
			if(v.value < saves.getmaximum())
				saves.set(v.value);
		} else
			abilitites[v.value] += v.counter;
	} else if(v.iskind<itemi>())
		items.set(v.value);
	else if(v.iskind<damagei>()) {
		switch(modifieri::last) {
		case Resist: resist.set(v.value); break;
		case Immunity: immunity.set(v.value); break;
		case Vulnerable: vulnerable.set(v.value); break;
		default: break;
		}
	} else if(v.iskind<skilli>())
		skills.set(v.value);
	else if(v.iskind<listi>()) {
		for(auto v : bsdata<listi>::elements[v.value].elements)
			apply(v);
	}
}

void moveable::apply(const variants& source) {
	for(auto v : source)
		apply(v);
}

void moveable::update() {
	load(basic);
}