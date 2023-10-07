#include "card.h"
#include "crt.h"
#include "strategy.h"
#include "stringbuilder.h"
#include "structure.h"
#include "unit.h"
#include "variant.h"

static void add_head(stringbuilder& sb, const char* id) {
	sb.addn("###%1", getnm(id));
}

static void add_value(stringbuilder& sb, const char* id, int bonus) {
	if(!bonus)
		return;
	sb.adds("%1%+2i", getnm(id), bonus);
}

static void add_value(stringbuilder& sb, abilitya& source) {
	for(auto i = (ability_s)0; i <= Goods; i = (ability_s)(i + 1))
		add_value(sb, bsdata<abilityi>::elements[i].getname(), source.abilities[i]);
}

static void add_script(stringbuilder& sb, const char* id, int bonus) {
	auto p = getdescription(id);
	if(!p)
		return;
	sb.addn("- ");
	sb.add(p, bonus);
}

static void add_unit(stringbuilder& sb, int value, int bonus) {
	auto p = getdescription("PlaceTroops");
	if(!p)
		return;
	auto& ei = bsdata<uniti>::elements[value];
	sb.addn("- ");
	sb.add(p, ei.getname(), bonus);
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
	add_value(sb, *p);
}