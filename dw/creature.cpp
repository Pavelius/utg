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

static void add_variant(answers& an, const void* object, variant v) {
	if(v.type == Pack) {
		char temp[260]; stringbuilder sb(temp);
		bsdata<packi>::get(v.value).getinfo(sb);
		an.add(v.getpointer(), temp);
	} else
		an.add(v.getpointer(), getnm(v.getid()));
}

static bool set_value(void* object, const char* result, const void* value) {
	auto p = (creature*)object;
	if(!value)
		return false;
	if(bsdata<packi>::source.have(value)) {
		auto pv = (packi*)value;
		for(auto v : pv->elements)
			set_value(object, result, v.getpointer());
	} else if(equal("wear", result)) {
		auto v = bsdata<itemi>::source.indexof(value);
		if(v != -1)
			p->additem(v);
	} else
		return false;
	return true;
}

void creature::generate() {
	for(auto& e : bsdata<advancement>()) {
		if(e.type && !ismatch(e.type))
			continue;
		e.apply(this, add_variant, set_value);
	}
	if(!logs::interactive)
		random_ability();
	else
		choose_abilities();
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