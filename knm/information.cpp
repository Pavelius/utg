#include "crt.h"
#include "strategy.h"
#include "stringbuilder.h"
#include "variant.h"

static void add_head(stringbuilder& sb, const char* id) {
	sb.addn("###%1", getnm(id));
}

static void add_script(stringbuilder& sb, const char* id, int bonus) {
	auto p = getdescription(id);
	if(!p)
		return;
	sb.addn("- ");
	sb.add(p, bonus);
}

static void add_script(stringbuilder& sb, const variants& source) {
	for(auto v : source)
		add_script(sb, v.getid(), v.counter);
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