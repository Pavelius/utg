#include "main.h"

static char standart_ability[] = {16, 15, 13, 12, 9, 8};

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

void creature::random_ability() {
	auto& ei = bsdata<classi>::get(type);
	ability_s random[6]; memcpy(random, ei.abilities, sizeof(random));
	zshuffle(random + 3, 3);
	if(d100() < 30)
		iswap(random[1], random[2]);
	for(auto i = 0; i < 6; i++)
		basic.abilities[random[i]] = standart_ability[i];
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
	if(!logs::interactive)
		random_ability();
	id = "Mistra";
	choose_equipment();
	setavatar("mistra");
}