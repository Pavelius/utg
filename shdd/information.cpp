#include "stringbuilder.h"
#include "creature.h"
#include "nametable.h"
#include "variant.h"

struct stringproc {
	const char*		id;
	fnprint			proc;
};

static void add_dice(stringbuilder& sb, const creature* p, const item& weapon) {
	auto damage = weapon.geti().damage;
	auto damage_count = 1;
	auto damage_bonus = p->get(MeleeDamage);
	sb.adds("%+1i", p->get(MeleeAttack));
	sb.adds(weapon.getname());
	sb.adds("%1id%2i", damage_count, damage);
	if(damage_bonus)
		sb.add("%+1i", damage_bonus);
}

void creature_getproperty(const void* object, variant v, stringbuilder& sb) {
	auto p = (creature*)object;
	if(v.iskind<abilityi>()) {
		switch(v.value) {
		case AC:
			sb.add("%1i", 10 + p->get(AC));
			break;
		case MeleeAttack:
			add_dice(sb, p, p->wears[MeleeAttack]);
			break;
		default:
			sb.add("%1i", p->get((ability_s)v.value));
			break;
		}
	}
}

template<> void ftstatus<dice>(const void* object, stringbuilder& sb) {
	auto p = (dice*)object;
	sb.add("%1id%2i", p->c, p->d);
	if(p->b)
		sb.add("%+1i", p->b);
}

template<> void ftstatus<item>(const void* object, stringbuilder& sb) {
	auto p = (item*)object;
	sb.add(p->getname());
	auto& ei = p->geti();
	sb.adds("(%Weight %1i", ei.slot);
	auto damage = p->getdamage();
	if(damage.d != 0) {
		sb.adds(", %Damage ");
		ftstatus<dice>(&damage, sb);
	}
	sb.add(")");
}

template<> void ftstatus<creature>(const void* object, stringbuilder& sb) {
	if(bsdata<creature>::source.haveio(object)) {
		auto p = (creature*)object;
		sb.add("%1 - %2", p->getname(), p->getkindname(), p->get(Level));
	} else if(bsdata<creature>::source.have(object)) {
		auto p = (creature*)bsdata<creature>::source.ptr(bsdata<creature>::source.indexof(object));
		if(p->isitem(object))
			ftstatus<item>(object, sb);
	}
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

void item::addname(stringbuilder& sb) const {
	sb.adds(getname());
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