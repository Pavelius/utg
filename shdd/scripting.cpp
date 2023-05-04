#include "answers.h"
#include "ability.h"
#include "creature.h"
#include "item.h"
#include "modifier.h"
#include "script.h"

static void item_add(int type, int count) {
	item it; it.create(type, count);
	player->equip(it);
}

template<> void fnscript<itemi>(int value, int bonus) {
	item_add(value, bonus);
}

template<> void fnscript<abilityi>(int value, int bonus) {
	if(!bonus)
		bonus = 1;
	switch(modifier) {
	case Permanent: player->basic.abilities[value] += bonus; break;
	default: player->abilities[value] += bonus; break;
	}
}

static void talent_roll(int bonus) {
}

static void* choose_answers(const char* title, const char* cancel, int bonus) {
	char temp[260]; stringbuilder sb(temp); sb.add(title);
	if(bonus > 0)
		sb.adds("(Left %1i)", bonus);
	if(player->isplayer())
		return an.choose(temp, cancel, 0);
	return an.random();
}

static void apply_one_of(const listi* p, const char* title, int bonus) {
	while(bonus > 0) {
		for(auto v : p->elements) {
			if(v.counter)
				an.add(&v, "%1%+2i", v.getname(), v.counter);
			else
				an.add(&v, v.getname());
		}
		auto pv = (variant*)choose_answers(title, 0, bonus);
		script::run(*pv);
		bonus--;
	}
}

static void apply_one_of(const char* id, int bonus) {
	auto p = bsdata<listi>::find(id);
	if(p)
		apply_one_of(p, getnm(p->id), bonus);
}

static void raise_ability(int bonus) {
	apply_one_of("RaiseAnyAbility", bonus);
}

static void raise_attack(int bonus) {
	fnscript<abilityi>(MeleeAttack, bonus);
	fnscript<abilityi>(RangeAttack, bonus);
}

static void raise_damage(int bonus) {
	fnscript<abilityi>(MeleeDamage, bonus);
	fnscript<abilityi>(RangeDamage, bonus);
}

static void raise_class_ability(int bonus) {
	apply_one_of(str("%1StatsRaise", bsdata<classi>::elements[player->kind].id), 1);
}

static void choose_usable(const char* title, unsigned& usable, wear_s w1, wear_s w2) {
	for(auto& e : bsdata<itemi>()) {
		if(e.wear != w1 && e.wear != w2)
			continue;
		if((usable & FG(&e - bsdata<itemi>::elements)) != 0)
			continue;
		an.add(&e, e.getname());
	}
	auto p = (itemi*)an.choose(title);
	if(p)
		usable |= FG(p - bsdata<itemi>::elements);
}

static void armor_mastery(int bonus) {
	choose_usable("ChooseArmor", player->usable, Torso, Torso);
}

static void weapon_mastery(int bonus) {
	choose_usable("ChooseWeapon", player->mastery, MeleeWeapon, RangedWeapon);
}

BSDATA(script) = {
	{"ArmorMastery", armor_mastery},
	{"Attack", raise_attack},
	{"Damage", raise_damage},
	{"RaiseAbility", raise_ability},
	{"RaiseClassStats", raise_class_ability},
	{"TalentRoll", talent_roll},
	{"WeaponMastery", weapon_mastery},
};
BSDATAF(script)