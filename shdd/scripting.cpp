#include "answers.h"
#include "ability.h"
#include "creature.h"
#include "item.h"
#include "gender.h"
#include "modifier.h"
#include "nametable.h"
#include "pushvalue.h"
#include "questlist.h"
#include "roll.h"
#include "script.h"
#include "special.h"
#include "state.h"

static int		special_attacks;
racei*			last_race;
static genderi*	last_gender;
static const void* last_result;

static int d100() {
	return rand() % 100;
}

static bool apply_quest() {
	if(!last_result)
		return true;
	else if(bsdata<creature>::have(last_result)) {
		player = (creature*)last_result;
		return false;
	}  else if(items.have(last_result)) {
		auto p = (item*)last_result;
		player->act(getnm("PlayerPickUp"), p->getname());
		player->addequip(*p);
		return false;
	} else if(bsdata<quest>::have(last_result)) {
		last_quest = (quest*)last_result;
		if(last_quest->tags)
			script_run(last_quest->tags);
	}
	return true;
}

static void pick_up_answers(int bonus) {
	for(auto& e : items) {
		if(!e)
			continue;
		if(e.getcount()>1)
			an.add(&e, getnm("PickUpMany"), e.getname(), e.getcount());
		else
			an.add(&e, getnm("PickUp"), e.getname());
	}
}

static item& create_item(int type, int count) {
	static item it;
	it.create(type, count);
	return it;
}

static void print_prompt(const quest* p) {
	auto header = p->getheader();
	if(header)
		answers::header = header;
	auto resid = p->getimage();
	if(resid)
		answers::resid = resid;
	if(p->text)
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
		if(p->tags && !script_allow(p->tags))
			continue;
		if(p->text[0] == '/') {
			auto pn = bsdata<script>::find(p->text+1);
			if(pn)
				pn->proc(0);
		} else
			an.add(p, p->text);
	}
	if(!an)
		an.add(0, getnm("Continue"));
}

static bool apply_next() {
	if(!last_quest || !last_questlist || !last_quest->next)
		return false;
	last_result = last_questlist->find(last_quest->next);
	return true;
}

static void ask_what_to_do() {
	char temp[260]; stringbuilder sb(temp);
	sb.add(getnm("WhatDoYouDo"), player->getname());
	last_result = an.choose(temp);
}

static void play_quest() {
	if(!an.console)
		return;
	apply_quest();
	while(last_quest) {
		print_prompt(last_quest);
		while(true) {
			print_answers(last_quest);
			ask_what_to_do();
			if(apply_quest())
				break;
		}
		if(!apply_next())
			break;
		apply_quest();
	}
}

static void normalize_bonus(int& bonus) {
	if(!bonus)
		bonus = 1;
}

template<> void fnscript<consumablei>(int value, int bonus) {
	normalize_bonus(bonus);
	player->consumables[value] += bonus;
}

template<> void fnscript<racei>(int value, int bonus) {
	last_race = bsdata<racei>::elements + value;
}

template<> void fnscript<classi>(int value, int bonus) {
	last_class = bsdata<classi>::elements + value;
}

template<> void fnscript<genderi>(int value, int bonus) {
	last_gender = bsdata<genderi>::elements + value;
}

template<> void fnscript<questlist>(int value, int bonus) {
	pushvalue push_change_player(action_change_player, true);
	last_questlist = bsdata<questlist>::elements + value;
	last_quest = last_questlist->find(bonus);
	play_quest();
}

template<> void fnscript<itemi>(int value, int bonus) {
	switch(modifier) {
	case Enviroment: items.add(create_item(value, bonus)); break;
	default: player->addequip(create_item(value, bonus)); break;
	}
}

template<> void fnscript<abilityi>(int value, int bonus) {
	normalize_bonus(bonus);
	switch(modifier) {
	case Permanent: player->basic.abilities[value] += bonus; break;
	default: player->abilities[value] += bonus; break;
	}
}

template<> void fnscript<statei>(int value, int bonus) {
	if(bonus >= 0)
		player->states.set(value);
	else
		player->states.remove(value);
}

template<> void fnscript<nametable>(int value, int bonus) {
	last_table_element = bsdata<nametable>::elements[value].random();
	script_run(last_table_element->elements);
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
		script_run(*pv);
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

static void special_attack(item& weapon, speciali& effect) {
}

static void special_attack(item& weapon) {
	if(!opponent)
		return;
	auto power = weapon.getpower();
	if(power.iskind<speciali>())
		special_attack(weapon, bsdata<speciali>::elements[power.value]);
}

static void single_attack_enemy(ability_s attack, ability_s attack_damage, int advantage, item& weapon) {
	roll20(advantage);
	auto critical_miss = (roll_result <= 1);
	auto critical_hit = (roll_result >= 20);
	auto to_hit = roll_result + player->get(attack);
	auto ac = opponent->get(AC);
	if(critical_miss || to_hit < ac) {
		player->act(getnm("PlayerMiss"));
		return;
	}
	auto damage = player->getdamage(attack).roll();
	if(critical_hit)
		damage *= 2;
	player->act(getnm("PlayerHit"));
	opponent->damage(damage);
	special_attack(weapon);
}

static void attack_enemy(ability_s attack, ability_s attack_damage, int advantage, item& weapon) {
	pushvalue push(special_attacks, 0);
	auto number_of_attacks = weapon.getattacks();
	for(auto i = 0; i < number_of_attacks; i++)
		single_attack_enemy(attack, attack_damage, advantage, weapon);
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

static bool chance_roll(int bonus) {
	if(bonus > 0)
		return d100() < bonus;
	return false;
}

static void chance_cursed(int bonus) {
	if(chance_roll(bonus))
		last_item->setcursed(1);
}

static void chance_masterwork(int bonus) {
	if(chance_roll(bonus))
		last_item->setmasterwork(1);
}

static void chance_benefit(int bonus) {
	if(chance_roll(bonus)) {
	}
}

static void look_enemy(int bonus) {
	opponent = getbs<creature>(player->enemy);
	if(!opponent)
		opponent = player->getenemy();
	if(!opponent)
		script_stop();
	player->enemy = getbsi(opponent);
}
static bool if_look_enemy(int bonus) {
	return player->getenemy() != 0;
}

static bool if_melee_weapon(int bonus) {
	return player->wears[MeleeWeapon].operator bool();
}

static void damage_item(int bonus) {
	if(bonus >= 0)
		last_item->damage();
}

static void if_have(int bonus) {
	auto v = *script_begin++;
	if(v.iskind<statei>()) {
		if(bonus >= 0) {
			if(!player->states.is(v.value))
				script_stop();
		} else {
			if(player->states.is(v.value))
				script_stop();
		}
	}
}

static creature* find_npc(short unsigned location, int bonus) {
	for(auto& e : bsdata<creature>()) {
		if(e.location == location && e.npc_index == bonus)
			return &e;
	}
	return 0;
}

static void create_npc(int bonus) {
	pushvalue push(player);
	player = find_npc(0, bonus);
	if(!player) {
		add_npc_creature();
		player->npc_index = (char)bonus;
	}
	opponent = player;
}

BSDATA(script) = {
	{"ArmorMastery", armor_mastery},
	{"Attack", raise_attack},
	{"DamageItem", damage_item},
	{"ChanceBenefit", chance_benefit},
	{"ChanceCursed", chance_cursed},
	{"ChanceMasterwork", chance_masterwork},
	{"CreateNPC", create_npc},
	{"Damage", raise_damage},
	{"IfHave", if_have},
	{"LookEnemy", look_enemy, if_look_enemy},
	{"MakeIdentify", make_identify},
	{"MakeMeleeAttack", make_melee_attack, if_melee_weapon},
	{"MakeRangeAttack", make_range_attack},
	{"PickUpAnswers", pick_up_answers},
	{"RaiseAbility", raise_ability},
	{"RaiseClassStats", raise_class_ability},
	{"TalentRoll", talent_roll},
	{"WeaponMastery", weapon_mastery},
};
BSDATAF(script)