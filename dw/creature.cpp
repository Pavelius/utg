#include "main.h"

static char standart_ability[] = {16, 15, 13, 12, 9, 8};
static gender_s last_gender;

const char* creature::getavatarst(const void* p) {
	if(!((creature*)p)->isvalidname())
		return 0;
	return ((creature*)p)->avatarable::getavatar();
}

void creature::update() {
	copy(basic);
}

void creature::update_class(classi& e) {
	abilities[HP] += e.damage;
}

void creature::finish() {
	update();
	update_player();
	update_class(bsdata<classi>::get(type));
	hp = get(HP);
}

void creature::random_ability() {
	auto& ei = bsdata<classi>::get(type);
	ability_s random[6]; memcpy(random, ei.abilities, sizeof(random));
	zshuffle(random + 2, 4);
	if(d100() < 30)
		iswap(random[1], random[2]);
	for(auto i = 0; i < 6; i++)
		basic.abilities[random[i]] = standart_ability[i];
}

void creature::choose_abilities() {
	for(auto v : standart_ability)
		basic.apply_ability(v);
}

void creature::choose_name() {
	clearname();
	short unsigned temp[512];
	variant source[3] = {variant(Gender, last_gender), variant(Class, type), variant(Race, race)};
	auto count = charname::select(temp, temp + sizeof(temp) / sizeof(temp[0]), source);
	if(!count)
		count = charname::select(temp, temp + sizeof(temp) / sizeof(temp[0]), slice<variant>(source, 2));
	if(!count)
		return;
	answers an;
	for(auto v : slice<short unsigned>(temp, count))
		an.add((void*)v, charname::getname(v));
	setname((int)an.choose("Как вас зовут?", 0));
}

static bool match_party(variant v) {
	for(auto& e : bsdata<creature>()) {
		if(e.ismatch(v))
			return true;
	}
	return false;
}

static void add_variant(answers& an, const void* object, variant v) {
	if(v.type == Class) {
		if(!match_party(v))
			an.add(v.getpointer(), getnm(v.getid()));
	} else if(v.type == Pack) {
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
	if(bsdata<packi>::have(value)) {
		auto pv = (packi*)value;
		for(auto v : pv->elements)
			set_value(object, result, v.getpointer());
	} else if(equal("wear", result)) {
		auto v = bsdata<itemi>::source.indexof(value);
		if(v != -1)
			p->additem(v);
	} else if(bsdata<genderi>::have(value))
		last_gender = (gender_s)bsdata<genderi>::source.indexof(value);
	else
		return false;
	return true;
}

void creature::choose_avatar() {
	stringbuilder sb(avatar);
	//utg::chooseavatar(sb, getnm("ChooseLook"), (last_gender == Female) ? "f*.*" : "m*.*");
}

void creature::generate() {
	for(auto& e : bsdata<advancement>()) {
		if(e.type && !ismatch(e.type))
			continue;
		e.apply(this, add_variant, set_value);
	}
	if(!answers::interactive)
		random_ability();
	else
		choose_abilities();
	choose_name();
	choose_avatar();
	finish();
}

dice creature::getdamage() const {
	dice result = {1, 4};
	auto damage = getclass().damage;
	if(damage)
		result.d = damage;
	return result;
}

bool creature::ismatch(variant v) const {
	switch(v.type) {
	case Class: return type == v.value;
	case Gender: return getgender() == v.value;
	case Race: return race == v.value;
	default: return false;
	}
}