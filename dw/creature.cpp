#include "assign.h"
#include "bsreq.h"
#include "class.h"
#include "item.h"
#include "list.h"
#include "race.h"
#include "creature.h"

static char standart_ability[] = {16, 15, 13, 12, 9, 8};
static gender_s last_gender;

creature* player;

static int d100() {
	return rand() % 100;
}

const classi& creature::geti() const {
	return bsdata<classi>::elements[type];
}

int creature::getmaximumhp() const {
	return get(Constitution) + geti().damage;
}

static void getinfo(variant v, stringbuilder& sb) {
	if(v.iskind<itemi>()) {
		auto p = bsdata<itemi>::elements + v.value;
		if(p->tags.is(Coins))
			sb.add("%1i %Coins", p->coins * v.counter);
		else
			sb.add(getnm(p->id));
	} else
		sb.add(v.getname());
}

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
			getinfo(v, sb);
		else
			sb.add("%1i %-2", count, v.getname());
		count = 1;
	}
}

void creature::update() {
	assign(*static_cast<statable*>(this), basic);
}

void creature::finish() {
	update();
	update_player();
	abilities[HP] = getmaximumhp();
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
	auto count = stringlist::select(temp, temp + sizeof(temp) / sizeof(temp[0]), str("%1%2%3",
		bsdata<racei>::elements[race].id,
		bsdata<genderi>::elements[last_gender].id,
		bsdata<classi>::elements[type].id));
	if(!count)
		return;
	answers an;
	for(auto v : slice<short unsigned>(temp, count))
		an.add((void*)v, stringlist::getname(v));
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
	} else {
		char temp[260]; stringbuilder sb(temp);
		getinfo(v, sb);
		an.add(v.getpointer(), temp);
	}
}

static bool set_value(void* object, const char* result, const void* value, int count) {
	auto p = (creature*)object;
	if(!value)
		return false;
	if(bsdata<listi>::have(value)) {
		auto pv = (listi*)value;
		for(auto v : pv->elements)
			set_value(object, result, v.getpointer(), v.counter);
	} else if(equal("wear", result)) {
		auto pi = (itemi*)value;
		if(pi->tags.is(Coins))
			p->setcoins(p->getcoins() + count);
		else {
			item it(bsdata<itemi>::source.indexof(value));
			p->additem(it);
		}
	} else if(bsdata<genderi>::have(value))
		last_gender = (gender_s)bsdata<genderi>::source.indexof(value);
	else if(bsdata<variant>::have(value)) {
		auto p = (variant*)value;
		return set_value(object, result, p->getpointer(), p->counter);
	} else
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
	auto damage = geti().damage;
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

creature* party_maximum(ability_s v, tag_s t) {
	creature* result = 0;
	for(auto& e : bsdata<creature>()) {
		if(!e)
			continue;
		if(result && result->get(v) >= e.get(v))
			continue;
		result = &e;
	}
	return result;
}

creature* party_maximum(ability_s v) {
	creature* result = 0;
	for(auto& e : bsdata<creature>()) {
		if(!e)
			continue;
		if(result && result->get(v) >= e.get(v))
			continue;
		result = &e;
	}
	return result;
}

const char* party_avatar(const void* p) {
	if(!((creature*)p)->isvalidname())
		return 0;
	return ((creature*)p)->avatarable::getavatar();
}