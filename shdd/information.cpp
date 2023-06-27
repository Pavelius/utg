#include "stringbuilder.h"
#include "creature.h"
#include "nametable.h"
#include "variant.h"

struct stringproc {
	const char*	id;
	fnprint		proc;
};

void creature_getproperty(const void* object, variant v, stringbuilder& sb) {
	auto p = (creature*)object;
	if(v.iskind<abilityi>()) {
		switch(v.value) {
		case AC:
			sb.add("%1i", 10+p->get(AC));
			break;
		default:
			sb.add("%1i", p->get((ability_s)v.value));
			break;
		}
	}
}

template<> void ftstatus<creature>(const void* object, stringbuilder& sb) {
	auto p = (creature*)object;
	sb.add("%1 - %2", p->getname(), p->getkindname(), p->get(Level));
}

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