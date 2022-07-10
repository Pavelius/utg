#include "main.h"

modifier_s modifieri::last;

void moveable::apply(const variants& source) {
	for(auto v : source)
		addstart(v, Proficient, true);
}

bool moveable::addstart(variant v, modifier_s modifier, bool run) {
	if(v.iskind<racei>()) {
		if(run)
			race = (racei*)v.getpointer();
	} else if(v.iskind<genderi>()) {
		if(run)
			gender = (gender_s)v.value;
	} else if(v.iskind<alignmenti>()) {
		if(run)
			alignment = (unsigned char)v.value;
	} else
		return statable::addstart(v, modifier, run);
	return true;
}

static void add_variant(moveable* p, answers& an, const variants& source, modifier_s modifier) {
	for(auto v : source) {
		if(!p->addstart(v, modifier, false))
			continue;
		an.add(v.getpointer(), v.getname());
	}
}

void moveable::apply(const advancei& source) {
	for(auto i = 0; i < source.choose; i++) {
		answers an;
		add_variant(this, an, source.effect, Proficient);
		auto pv = an.choose(getnm(source.id));
		if(!pv)
			break;
		addstart(pv, Proficient, true);
	}
}

void moveable::advance(variant base, int level) {
	for(auto& e : bsdata<advancei>()) {
		if(e.base == base && e.level == level)
			apply(e);
	}
}

void moveable::update() {
	load(basic);
}