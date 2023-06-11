#include "stringbuilder.h"
#include "creature.h"
#include "nametable.h"

struct stringproc {
	const char*	id;
	fnprint		proc;
};

static void treasure_text(stringbuilder& sb) {
	if(!last_table_element)
		sb.add("NoTreasure");
	else
		sb.add(last_table_element->name);
}

static void weapon_text(stringbuilder& sb) {
	if(!player)
		sb.add(getnm("NoPlayer"));
	else if(player->wears[MeleeWeapon])
		sb.add(player->wears[MeleeWeapon].getname());
	else
		sb.add(getnm("Hands"));
}

BSDATA(stringproc) = {
	{"TreasureText", treasure_text},
	{"Weapon", weapon_text},
};
BSDATAF(stringproc)

void stringbuilder_proc(stringbuilder& sb, const char* id) {
	auto pf = bsdata<stringproc>::find(id);
	if(pf) {
		pf->proc(sb);
		return;
	}
	sb.defidentifier(sb, id);
}