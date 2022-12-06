#include "advance.h"
#include "alignment.h"
#include "class.h"
#include "modifier.h"
#include "race.h"
#include "moveable.h"

//static racei* choose_race(const racei* parent) {
//	answers an;
//	for(auto& e : bsdata<racei>()) {
//		if(e.parent == parent)
//			an.add(&e, getnm(e.id));
//	}
//	return (racei*)an.choose(getnm("ChooseSubRace"));
//}
//
//bool moveable::addstart(variant v, modifier_s modifier, bool run) {
//	if(v.iskind<racei>()) {
//		if(run) {
//			auto p = (racei*)v.getpointer();
//			auto pn = choose_race(p);
//			setkind(pn ? pn : p);
//		}
//	} else if(v.iskind<genderi>()) {
//		if(run)
//			setgender((gender_s)v.value);
//	} else if(v.iskind<alignmenti>()) {
//		if(run)
//			alignment = (unsigned char)v.value;
//	} else if(v.iskind<classi>()) {
//		if(run)
//			classes[v.value]++;
//	} else
//		return statable::addstart(v, modifier, run);
//	return true;
//}
//
//static void add_variant(moveable* p, answers& an, const variants& source, modifier_s& modifier) {
//	for(auto v : source) {
//		if(v.iskind<modifieri>())
//			modifier = (modifier_s)v.value;
//		else {
//			if(!p->addstart(v, modifier, false))
//				continue;
//			an.add(v.getpointer(), v.getname());
//		}
//	}
//}
//
//void moveable::apply(const advancei& source) {
//	char temp[260]; stringbuilder sb(temp);
//	for(auto i = 0; i < source.choose; i++) {
//		answers an;
//		auto modifier = Proficient;
//		add_variant(this, an, source.elements, modifier);
//		auto pn = source.id;
//		if(szstart(pn, "Choose"))
//			pn = getnm(pn);
//		else {
//			sb.clear();
//			sb.add("%Choose [%1]", getnm(source.id));
//			pn = temp;
//		}
//		auto pv = an.choose(pn);
//		if(!pv)
//			break;
//		addstart(pv, modifier, true);
//	}
//}
//
//void moveable::advance(variant object, int level) {
//	for(auto& e : bsdata<advancei>()) {
//		if(e.object == object && e.level == level)
//			apply(e);
//	}
//}
//
//void moveable::update() {
//	load(basic);
//}