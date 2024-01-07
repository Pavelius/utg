#include "ability.h"
#include "creature.h"
#include "stringbuilder.h"

static bool apply_ability(stringbuilder& sb, const char* id) {
	auto p = bsdata<abilityi>::find(id);
	if(!p)
		return false;
	auto format = "%1i";
	//if(p->format)
	//	format = p->format;
	sb.add(format, player->abilities[getbsi(p)]);
	return true;
}

static void custom_string(stringbuilder& sb, const char* id) {
	if(apply_ability(sb, id))
		return;
}

void initialize_str() {
	stringbuilder::custom = custom_string;
}