#include "bsreq.h"
#include "list.h"
#include "main.h"

static char standart_ability[] = {16, 15, 13, 12, 9, 8};
static gender_s last_gender;

creature* player;

static void getinfo(const variants& elements, stringbuilder& sb) {
	auto m = elements.count;
	variant last = 0;
	int count = 1;
	auto pbg = sb.get(); pbg[0] = 0;
	auto pse = elements.begin();
	for(unsigned i = 0; i < m; i++) {
		auto v = pse[i];
		if(i != (m - 1) && v == pse[i + 1]) {
			count++;
			continue;
		}
		last = v;
		if(pbg[0]) {
			if(i == (m - 1))
				sb.add(" и ");
			else
				sb.add(", ");
		}
		if(count == 1)
			sb.add(v.getname());
		else
			sb.add("%1i %-2", count, v.getname());
		count = 1;
	}
}

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
	variant vgender = bsdata<genderi>::elements + last_gender;
	variant vclass = bsdata<classi>::elements + type;
	variant vrace = bsdata<racei>::elements + race;
	variant source[3] = {vgender, vclass, vrace};
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
	if(v.iskind<classi>()) {
		if(!match_party(v))
			an.add(v.getpointer(), v.getname());
	} else if(v.iskind<listi>()) {
		char temp[260]; stringbuilder sb(temp);
		getinfo(bsdata<listi>::elements[v.value].elements, sb);
		an.add(v.getpointer(), temp);
	} else
		an.add(v.getpointer(), v.getname());
}

static bool set_value(void* object, const char* result, const void* value) {
	auto p = (creature*)object;
	if(!value)
		return false;
	if(bsdata<listi>::have(value)) {
		auto pv = (listi*)value;
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
	auto pn = avatarable::choose(getnm("ChooseLook"), (last_gender == Female) ? "f*.*" : "m*.*", 6);
	if(pn)
		setavatar(pn);
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
	if(v.iskind<classi>())
		return type == v.value;
	else if(v.iskind<genderi>())
		return getgender() == v.value;
	else if(v.iskind<racei>())
		return race == v.value;
	else
		return false;
}