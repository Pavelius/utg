#include "card.h"
#include "crt.h"
#include "strategy.h"
#include "stringbuilder.h"
#include "structure.h"
#include "troop.h"
#include "unit.h"
#include "upgrade.h"
#include "variant.h"

static void add_line(stringbuilder& sb) {
	sb.addn("---");
}

static void add_head(stringbuilder& sb, const char* id) {
	if(!id)
		return;
	sb.addn("###%1", getnm(id));
}

static void add_value(stringbuilder& sb, const char* id, int bonus) {
	if(!bonus)
		return;
	sb.adds("%1%+2i", getnm(id), bonus);
}

static void add_value(stringbuilder& sb, const char* prefix, const char* id, int bonus) {
	if(!bonus)
		return;
	auto p = getdescription(stw(prefix, id, "Tips"));
	if(!p)
		return;
	sb.addn(p, bonus);
}

static const char* get_ten_times(int value) {
	static char temp[32]; stringbuilder sb(temp);
	auto number = value / 10;
	auto fraction = value % 10;
	if(fraction)
		sb.add("%1i.%2i", number, fraction);
	else
		sb.add("%1i", number);
	return temp;
}

static const char* get_times(const char* id, int value) {
	static char temp[32]; stringbuilder sb(temp);
	sb.add("%1%2i", id, value);
	auto p = getdescription(temp);
	if(!p)
		return "";
	sb.clear();
	sb.add(" %1", p);
	return temp;
}

static void add_value_unit(stringbuilder& sb, ability_s i, int bonus, int bonus2 = 0) {
	if(!bonus)
		return;
	auto id = bsdata<abilityi>::elements[i].id;
	auto p = getdescription(stw("Unit", id, "Tips"));
	if(!p)
		return;
	switch(i) {
	case Army:
		sb.addn(p, get_ten_times(bonus));
		break;
	case Combat:
		sb.addn(p, bonus, get_times("Times", bonus2));
		break;
	default:
		sb.addn(p, bonus);
		break;
	}
}

static void add_value(stringbuilder& sb, abilitya& source) {
	for(auto i = (ability_s)0; i <= Gold; i = (ability_s)(i + 1))
		add_value(sb, bsdata<abilityi>::elements[i].id, source.abilities[i]);
}

static void add_value(stringbuilder& sb, abilitya& source, const char* prefix) {
	for(auto i = (ability_s)0; i <= Gold; i = (ability_s)(i + 1))
		add_value(sb, prefix, bsdata<abilityi>::elements[i].id, source.abilities[i]);
}

static void add_value_unit(stringbuilder& sb, abilitya& source) {
	for(auto i = (ability_s)0; i <= Gold; i = (ability_s)(i + 1)) {
		switch(i) {
		case Combat:
			add_value_unit(sb, i, source.abilities[i], source.abilities[Damage]);
			break;
		default:
			add_value_unit(sb, i, source.abilities[i]);
			break;
		}
	}
}

static void add_script(stringbuilder& sb, const char* id, int bonus) {
	auto p = getdescription(id);
	if(!p)
		return;
	sb.addn("- ");
	sb.add(p, bonus);
}

static void add_script(stringbuilder& sb, const char* id, const char* name, int bonus) {
	auto p = getdescription(id);
	if(!p)
		return;
	sb.addn("- ");
	sb.add(p, bonus, name);
}

static void add_unit(stringbuilder& sb, int value, int bonus) {
	add_script(sb, "PlaceTroops", bsdata<uniti>::elements[value].getname(), bonus);
}

static void add_script(stringbuilder& sb, const variants& source) {
	for(auto v : source) {
		if(v.iskind<uniti>())
			add_unit(sb, v.value, v.counter);
		else
			add_script(sb, v.getid(), v.counter);
	}
}

static void add_ability(stringbuilder& sb, const variants& source, const char* id) {
	if(!source)
		return;
	add_head(sb, id);
	add_script(sb, source);
}

static void add_textinfo(stringbuilder& sb, const char* id) {
	auto p = getdescription(id);
	if(p)
		sb.addn(p);
}

template<> void ftinfo<strategyi>(const void* object, stringbuilder& sb) {
	auto p = (strategyi*)object;
	add_ability(sb, p->primary, "PrimaryAbility");
	add_ability(sb, p->secondary, "SecondaryAbility");
}

template<> void ftinfo<cardi>(const void* object, stringbuilder& sb) {
	auto p = (cardi*)object;
	add_ability(sb, p->effect, "Effect");
}

template<> void ftinfo<structurei>(const void* object, stringbuilder& sb) {
	auto p = (structurei*)object;
	add_head(sb, p->id);
	add_line(sb);
	add_value(sb, *p, "Structure");
}

template<> void ftinfo<structure>(const void* object, stringbuilder& sb) {
	auto p = (structure*)object;
	ftinfo<structurei>(p->id, sb);
}

template<> void ftinfo<uniti>(const void* object, stringbuilder& sb) {
	auto p = (uniti*)object;
	add_head(sb, p->id);
	add_line(sb);
	add_value_unit(sb, *p);
}

template<> void ftinfo<troopi>(const void* object, stringbuilder& sb) {
	auto p = (troopi*)object;
	ftinfo<uniti>(p->id, sb);
}

template<> void ftinfo<upgradei>(const void* object, stringbuilder& sb) {
	auto p = (upgradei*)object;
	add_head(sb, p->id);
	add_textinfo(sb, p->id);
}

const char* getinform(const void* object) {
	static char temp[512];
	stringbuilder sb(temp); sb.clear();
	if(bsdata<upgradei>::have(object))
		ftinfo<upgradei>(object, sb);
	return temp;
}