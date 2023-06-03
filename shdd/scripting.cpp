#include "answers.h"
#include "ability.h"
#include "creature.h"
#include "item.h"
#include "modifier.h"
#include "nametable.h"
#include "questlist.h"
#include "roll.h"
#include "script.h"

static int d100() {
	return rand() % 100;
}

static item& create_item(int type, int count) {
	static item it;
	it.create(type, count);
	return it;
}

static void print_prompt(const quest* p) {
	if(!p->text)
		return;
	an.console->add(p->text);
}

static void print_answers(const quest* pb) {
	an.clear();
	auto index = pb->index;
	auto pe = last_questlist->elements.end();
	for(auto p = pb + 1; p < pe; p++) {
		if(p->index != index)
			break;
		if(!p->text)
			continue;
		if(p->tags && !script::allow(p->tags))
			continue;
		an.add(p, p->text);
	}
	if(!an)
		an.add(0, getnm("Continue"));
}

static void apply_quest() {
	if(!last_quest || !last_quest->tags)
		return;
	script::run(last_quest->tags);
}

static bool apply_next() {
	if(!last_quest || !last_questlist)
		return false;
	last_quest = last_questlist->find(last_quest->next);
	return true;
}

static void play_quest() {
	if(!an.console)
		return;
	apply_quest();
	while(last_quest) {
		print_prompt(last_quest);
		print_answers(last_quest);
		last_quest = (quest*)an.choose(getnm("WhatDoYouDo"));
		apply_quest();
		if(!apply_next())
			break;
		apply_quest();
	}
}

template<> void fnscript<questlist>(int value, int bonus) {
	last_questlist = bsdata<questlist>::elements + value;
	last_quest = last_questlist->find(bonus);
	play_quest();
}

template<> void fnscript<itemi>(int value, int bonus) {
	switch(modifier) {
	case Enviroment: items.add(create_item(value, bonus)); break;
	default: player->equip(create_item(value, bonus)); break;
	}
}

template<> void fnscript<abilityi>(int value, int bonus) {
	if(!bonus)
		bonus = 1;
	switch(modifier) {
	case Permanent: player->basic.abilities[value] += bonus; break;
	default: player->abilities[value] += bonus; break;
	}
}

template<> void fnscript<nametable>(int value, int bonus) {
	auto p = bsdata<nametable>::elements[value].random();
	script::run(p->elements);
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
	choose_usable("ChooseArmorMastery", player->mastery, Torso, Torso);
}

static void weapon_mastery(int bonus) {
	choose_usable("ChooseWeaponMastery", player->mastery, MeleeWeapon, RangedWeapon);
}

static void attack_enemy(ability_s attack, ability_s attack_damage, int advantage, item& weapon) {
	roll20(advantage);
	auto critical_miss = (roll_result <= 1);
	auto critical_hit = (roll_result >= 20);
	auto to_hit = roll_result + player->get(attack);
	auto ac = opponent->get(AC);
	if(critical_miss || to_hit < ac) {
		player->act("PlayerMiss");
		return;
	}
	auto damage = player->getdamage(attack).roll();
	if(critical_hit)
		damage *= 2;
	player->act("PlayerHit");
	opponent->damage(damage);
}

static void make_melee_attack(int bonus) {
	attack_enemy(MeleeAttack, MeleeDamage, 0, player->wears[MeleeWeapon]);
}

static void make_range_attack(int bonus) {
	attack_enemy(RangeAttack, RangeDamage, 0, player->wears[RangedWeapon]);
}

static void make_identify(int bonus) {
	last_item->setidentify(bonus);
}

static bool chanceroll(int bonus) {
	if(bonus > 0)
		return d100() < bonus;
	return false;
}

static void chance_cursed(int bonus) {
	if(chanceroll(bonus))
		last_item->setcursed(1);
}

static void chance_benefit(int bonus) {
	if(chanceroll(bonus)) {
	}
}

static void look_enemy(int bonus) {
	opponent = getbs<creature>(player->enemy);
	if(!opponent)
		opponent = player->getenemy();
	if(!opponent)
		script::stop();
	player->enemy = getbsi(opponent);
}
static bool if_look_enemy(int bonus) {
	return player->getenemy() != 0;
}

static bool if_melee_weapon(int bonus) {
	return player->wears[MeleeWeapon].operator bool();
}

BSDATA(script) = {
	{"ArmorMastery", armor_mastery},
	{"Attack", raise_attack},
	{"ChanceBenefit", chance_benefit},
	{"ChanceCused", chance_cursed},
	{"Damage", raise_damage},
	{"LookEnemy", look_enemy, if_look_enemy},
	{"MakeIdentify", make_identify},
	{"MakeMeleeAttack", make_melee_attack, if_melee_weapon},
	{"MakeRangeAttack", make_range_attack},
	{"RaiseAbility", raise_ability},
	{"RaiseClassStats", raise_class_ability},
	{"TalentRoll", talent_roll},
	{"WeaponMastery", weapon_mastery},
};
BSDATAF(script)