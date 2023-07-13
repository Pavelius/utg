#include "stringbuilder.h"
#include "creature.h"
#include "nametable.h"
#include "variant.h"

struct stringproc {
	const char*	id;
	fnprint		proc;
};

static void add_dice(stringbuilder& sb, const creature* p, const item& weapon) {
	auto damage = weapon.geti().damage;
	auto damage_count = 1;
	auto damage_bonus = p->get(MeleeDamage);
	sb.adds("%+1i", p->get(MeleeAttack));
	// sb.adds(weapon.getname());
	sb.adds("%1id%2i", damage_count, damage);
	if(damage_bonus)
		sb.add("%+1i", damage_bonus);
}

static void add_attack(stringbuilder& sb, const creature* p, ability_s attack, ability_s damage) {
	sb.adds("%+1i %ToHit", p->get(attack));
	auto i = p->get(damage);
	if(i)
		sb.adds("%+1i %ToDamage", i);
}

void creature_getproperty(const void* object, variant v, stringbuilder& sb) {
	auto p = (creature*)object;
	if(v.iskind<abilityi>()) {
		switch(v.value) {
		case AC:
			sb.add("%1i", 10 + p->get(AC));
			break;
		case MeleeAttack:
			add_attack(sb, p, MeleeAttack, MeleeDamage);
			break;
		case RangeAttack:
			add_attack(sb, p, RangeAttack, RangeDamage);
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

void item::getinfo(stringbuilder& sb) const {
	auto po = getowner(*this);
	sb.add(getname());
	auto& ei = geti();
	sb.adds("(%Weight %1i", ei.slot);
	auto damage = getdamage();
	if(damage.d != 0) {
		sb.adds(", %Damage ");
		ftstatus<dice>(&damage, sb);
	}
	sb.add(")");
}

void creature::getinfo(stringbuilder& sb) const {
	if(bsdata<creature>::source.haveio(this)) {
		sb.add("%1 - %2", getname(), getkindname(), get(Level));
	} else if(bsdata<creature>::source.have(this)) {
		auto p = (creature*)bsdata<creature>::source.ptr(bsdata<creature>::source.indexof(this));
		if(p->isitem(this))
			((item*)this)->getinfo(sb);
	}
}

void item::addname(stringbuilder& sb) const {
	sb.adds(getname());
}

static void treasure_text(stringbuilder& sb) {
	if(!last_table_element)
		sb.add(getnm("NoTreasure"));
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

static void hero_name(stringbuilder& sb) {
	if(!player)
		sb.add(getnm("NoPlayer"));
	else
		sb.add(player->getname());
}

BSDATA(stringproc) = {
	{"Name", hero_name},
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