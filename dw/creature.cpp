#include "main.h"

static char standart_ability[] = {16, 15, 13, 12, 9, 8};

void creature::update() {
	copy(basic);
}

void creature::finish() {
	update();
	update_player();
	hp = get(HP);
}

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

static variant choose_advance(startequipmenti& se) {
	answers an;
	if(se.elements) {
		for(auto& e1 : se.elements) {
			if(e1.type == Pack) {
				char temp[260]; stringbuilder sb(temp);
				bsdata<packi>::get(e1.value).getinfo(sb);
				an.add((void*)((int)e1), temp);
			} else
				an.add((void*)((int)e1), getnm(e1.getid()));
		}
	}
	return (int)an.choose(getnm(se.id), 0, logs::interactive, logs::url, 1, logs::header);
}

static void apply_advance_option(void* object, varianti& type, startequipmenti& se) {
	auto v = choose_advance(se);
	type.set(object, se.result, v.getpointer());
}

void creature::apply_advance() {
	for(auto& e : bsdata<startequipmenti>())
		apply_advance_option(this, bsdata<varianti>::elements[Creature], e);
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

void creature::choose_abilities() {
	for(auto v : standart_ability)
		basic.apply_ability(v);
}

void creature::generate() {
	for(auto& e : bsdata<advancement>()) {
		if(e.type && !ismatch(e.type))
			continue;
		e.apply(this);
	}
	if(!logs::interactive)
		random_ability();
	else
		choose_abilities();
	choose_equipment();
	id = "Mistra";
	setavatar("mistra");
	finish();
}

bool creature::ismatch(variant v) const {
	switch(v.type) {
	case Class: return type == v.value;
	case Gender: return gender == v.value;
	case Race: return race == v.value;
	default: return false;
	}
}