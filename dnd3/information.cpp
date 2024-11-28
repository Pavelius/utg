#include "ability.h"
#include "creature.h"
#include "stringbuilder.h"
#include "stringvar.h"

int calculate(statable* p, variants source);

static bool apply_ability(stringbuilder& sb, const char* id) {
	auto p = bsdata<abilityi>::find(id);
	if(!p)
		return false;
	auto format = "%1i";
	if(p->format)
		format = p->format;
	sb.add(format, player->abilities[getbsi(p)]);
	return true;
}

static bool apply_formula(stringbuilder& sb, const char* id) {
	auto p = bsdata<formulai>::find(id);
	if(!p)
		return false;
	auto format = "%1i";
	if(p->format)
		format = p->format;
	sb.add(format, player->abilities[getbsi(p)]);
	return true;
}

static void custom_string(stringbuilder& sb, const char* id) {
	if(apply_ability(sb, id))
		return;
	if(stringvar_identifier(sb, id))
		return;
	default_string(sb, id);
}

void initialize_str() {
	stringbuilder::custom = custom_string;
}

static void weapon_damage(stringbuilder& sb) {
	auto& e = player->wears[Hands];
	bsdata<rolli>::elements[e.geti().weapon.damage].add(sb);
}

BSDATA(stringvari) = {
	{"damage", weapon_damage},
};
BSDATAF(stringvari)