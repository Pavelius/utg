#include "ability.h"
#include "action.h"
#include "answers.h"
#include "creature.h"
#include "list.h"
#include "modifier.h"
#include "pushvalue.h"
#include "script.h"
#include "skill.h"

static void* last_answer;
static const char* last_id;
static int last_roll;

template<> void fnscript<modifieri>(int value, int bonus) {
	modifier = (modifier_s)value;
}

template<> void fnscript<weari>(int value, int bonus) {
	last_wear = (wear_s)value;
}
template<> bool fntest<weari>(int value, int bonus) {
	fnscript<weari>(value, bonus);
	return true;
}

template<> void fnscript<skilli>(int value, int bonus) {
	last_skill = (skill_s)value;
}
template<> bool fntest<skilli>(int value, int bonus) {
	fnscript<skilli>(value, bonus);
	return true;
}

static void reduce_ability(char& source, int& bonus) {
	if(!source || !bonus)
		return;
	if(source >= -bonus) {
		source += bonus;
		bonus = 0;
	} else {
		bonus += source;
		source = 0;
	}
}

static void reduce_ability(int value, int bonus) {
	switch(value) {
	case SwiftAction:
		reduce_ability(player->abilities[SwiftAction], bonus);
		reduce_ability(player->abilities[MoveAction], bonus);
		reduce_ability(player->abilities[StandartAction], bonus);
		break;
	case MoveAction:
		reduce_ability(player->abilities[MoveAction], bonus);
		reduce_ability(player->abilities[StandartAction], bonus);
		break;
	default:
		reduce_ability(player->abilities[value], bonus);
		break;
	}
}

static void add_ability(int value, int bonus) {
	if(bonus < 0)
		reduce_ability(value, bonus);
	else
		player->abilities[value] += bonus;
}

static bool allow_reduce_ability(int value, int bonus) {
	switch(value) {
	case MoveAction: return (player->abilities[MoveAction] + player->abilities[StandartAction]) >= -bonus;
	case SwiftAction: return (player->abilities[SwiftAction] + player->abilities[MoveAction] + player->abilities[StandartAction]) >= -bonus;
	default: return player->abilities[value] >= -bonus;
	}
}

static bool test_ability(int value, int bonus) {
	if(bonus < 0)
		return allow_reduce_ability(value, bonus);
	return true;
}

template<> void fnscript<abilityi>(int value, int bonus) {
	last_ability = (ability_s)value;
	switch(modifier) {
	case Permanent: player->basic.abilities[value] += bonus; break;
	default: add_ability(value, bonus); break;
	}
}
template<> bool fntest<abilityi>(int value, int bonus) {
	last_ability = (ability_s)value;
	switch(modifier) {
	case Permanent: return allow_reduce_ability(value, bonus);
	default: return allow_reduce_ability(value, bonus);
	}
}

static int roll20() {
	return 1 + rand() % 20;
}

void roll20(int bonus) {
	auto n = roll20();
	last_roll = roll20() + bonus;
	answers::console->add("[{%1i%+2i=%3i}]", n, bonus, last_roll);
}

static bool allow_combat() {
	for(auto p : creatures) {
		if(p->is(Hostile))
			return true;
	}
	return false;
}

static bool is_enemy(const void* object) {
	return player->isenemy((creature*)object);
}

static void filter_enemy(int bonus) {
	opponents.match(is_enemy, bonus >= 0);
}

static void choose_close_enemy(int bonus) {
	opponents = creatures;
	opponents.match(is_enemy, bonus >= 0);
	opponent = opponents.choose(0);
	if(!opponent)
		script_stop();
}

static void choose_near_enemy(int bonus) {
	opponents = creatures;
	opponents.match(is_enemy, bonus >= 0);
	opponent = opponents.choose(0);
	if(!opponent)
		script_stop();
}

static void choose_creature(int bonus) {
	player = creatures.choose(0, getnm("Cancel"));
}

static void choose_opponent(int bonus) {
	opponent = opponents.choose(0);
	if(!opponent)
		script_stop();
}

static bool if_train(int bonus) {
	return player->istrain(last_skill) == (bonus >= 0);
}

static bool if_choose_creature(int bonus) {
	last_script->proc(bonus);
	if(!opponents)
		return false;
	return true;
}

static bool if_full_round_action(int bonus) {
	return player->abilities[SwiftAction] > 0
		&& player->abilities[MoveAction] > 0
		&& player->abilities[StandartAction] > 0;
}
static void full_round_action(int bonus) {
	player->abilities[SwiftAction] = 0;
	player->abilities[MoveAction] = 0;
	player->abilities[StandartAction] = 0;
}

static void select_creatures(int bonus) {
	opponents = creatures;
}

static bool if_ready_wear(int bonus) {
	if(player->hands != last_wear)
		return false;
	return player->wears[player->hands].operator bool();
}
static bool if_ready_weapon(int bonus) {
	if(!(player->hands == MeleeWeapon || player->hands == RangedWeapon))
		return false;
	return player->wears[player->hands].operator bool();
}
static bool if_ready_item(int bonus) {
	if(player->hands == Backpack)
		return false;
	return player->wears[player->hands].operator bool();
}
static void ready_item(int bonus) {
	last_item = player->wears + player->hands;
}

static bool if_unarmed(int bonus) {
	return player->hands == Backpack;
}
static void unarmed(int bonus) {
	last_item = 0;
}

static void add_format(const char* format, const char separator = ' ') {
	if(!answers::console)
		return;
	answers::console->addsep(separator);
	answers::console->addv(format, 0);
}

static bool fix_message(stringbuilder& sb, const char* p1, const char* p2) {
	sb.clear();
	sb.add(last_id);
	sb.add(p1);
	sb.add(p2);
	auto p = getdescription(sb);
	if(!p)
		return false;
	add_format(p);
	return true;
}

static void fix_action(bool success) {
	char temp[128]; stringbuilder sb(temp);
	if(fix_message(sb, success ? "Success" : "Fail", 0))
		return;
}

static void make_attack(int bonus) {
	roll20(bonus);
	if(last_roll < opponent->get(Reflex)) {
		fix_action(false);
		script_stop();
		return;
	}
	fix_action(true);
}

static bool answers_have(const void* p) {
	for(auto& e : an) {
		if(e.value == p)
			return true;
	}
	return false;
}

static void add_actions() {
	for(auto& e : bsdata<actioni>()) {
		if(e.upgrade) {
			if(e.upgrade.iskind<feati>() && player->isfeat(e.upgrade.value))
				continue;
			else if(e.upgrade.iskind<actioni>() && answers_have(bsdata<actioni>::elements + e.upgrade.value))
				continue;
		}
		if(!script_allow(e.effect))
			continue;
		an.add(&e, e.getprompt());
		if(an.getcount() > 16)
			break;
	}
}

static void ask_answer() {
	last_answer = (void*)an.choose(getnm("WhatYouDo"));
}

static void apply_answers() {
	pushvalue push_id(last_id);
	if(bsdata<actioni>::have(last_answer)) {
		auto p = (actioni*)last_answer;
		last_id = p->id;
		script_run(p->effect);
	}
}

static void make_actions() {
	an.clear();
	add_actions();
	ask_answer();
	apply_answers();
}

static void before_combat_round() {
	player->abilities[Reaction] = 1;
	player->abilities[SwiftAction] = 1;
	player->abilities[MoveAction] = 1;
	player->abilities[StandartAction] = 1;
}

void one_combat_round() {
	pushvalue push(player);
	for(auto p : creatures) {
		player = p;
		before_combat_round();
		make_actions();
	}
}

BSDATA(script) = {
	{"ChooseCloseEnemy", choose_close_enemy, if_choose_creature},
	{"ChooseCreature", choose_creature},
	{"ChooseNearEnemy", choose_near_enemy, if_choose_creature},
	{"ChooseOpponent", choose_opponent, if_choose_creature},
	{"FilterEnemy", filter_enemy, choosing_script},
	{"FullRoundAction", full_round_action, if_full_round_action},
	{"IfTrained", conditional_script, if_train},
	{"MakeAttack", make_attack},
	{"ReadyItem", ready_item, if_ready_item},
	{"ReadyWeapon", ready_item, if_ready_weapon},
	{"ReadyWear", ready_item, if_ready_wear},
	{"SelectCreatures", select_creatures, choosing_script},
	{"Unarmed", unarmed, if_unarmed},
};
BSDATAF(script)