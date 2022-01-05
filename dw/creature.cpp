#include "main.h"

static variant choose_eqipment_option(startequipmenti& se) {
	answers an;
	for(auto& e1 : se.elements) {
		if(e1.type == Pack) {
			char temp[260]; stringbuilder sb(temp);
			bsdata<packi>::get(e1.value).getinfo(sb);
			an.add((void*)((int)e1), temp);
		} else
			an.add((void*)((int)e1), getnm(e1.getid()));
	}
	return (int)an.choose(getnm(se.id), 0, logs::interactive, logs::url, 1, logs::header);
}

void creature::choose_equipment() {
	for(auto& e : bsdata<startequipmenti>()) {
		if(e.type != type)
			continue;
		auto result = choose_eqipment_option(e);
	}
}

void creature::generate() {
	type = logs::choosei(bsdata<classi>::source,
		logs::getchoose("Class"), 0);
	alignment = logs::choosei(bsdata<alignmenti>::source,
		bsdata<classi>::elements[type].alignment,
		logs::getchoose("Alignment"), 0);
	gender = (gender_s)logs::choosei(bsdata<genderi>::source,
		bsdata<classi>::elements[type].gender,
		logs::getchoose("Gender"), 0);
	race = (race_s)logs::choosei(bsdata<racei>::source,
		bsdata<classi>::elements[type].race,
		logs::getchoose("Race"), 0);
	choose_equipment();
}