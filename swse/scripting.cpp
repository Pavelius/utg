#include "ability.h"
#include "action.h"
#include "answers.h"
#include "area.h"
#include "creature.h"
#include "list.h"
#include "modifier.h"
#include "pushvalue.h"
#include "rand.h"
#include "script.h"
#include "skill.h"

static void* last_answer;
static const char* last_id;
static int last_roll, pure_roll, critical_roll;

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

template<> void fnscript<statei>(int value, int bonus) {
	last_state = (state_s)value;
}

static int roll20() {
	return 1 + rand() % 20;
}

bool roll20(int bonus, int dc) {
	critical_roll = 0;
	pure_roll = roll20();
	last_roll = pure_roll + bonus;
	auto result = last_roll >= dc;
	if(pure_roll == 1) {
		critical_roll = -1;
		result = false;
	}
	if(pure_roll == 20) {
		critical_roll = 1;
		result = true;
	}
	if(result)
		answers::console->addn("[{%1i%+2i=%3i}]", pure_roll, bonus, last_roll);
	else
		answers::console->addn("[-{%1i%+2i=%3i}]", pure_roll, bonus, last_roll);
	return result;
}

static bool allow_combat() {
	for(auto p : creatures) {
		if(p->is(Hostile))
			return true;
	}
	return false;
}

static bool is_armed(const void* object) {
	return ((creature*)object)->wears[Hands].operator bool();
}
static bool is_enemy(const void* object) {
	return player->isenemy((creature*)object);
}
static bool is_player(const void* object) {
	return player == object;
}
static bool is_range(const void* object) {
	return player->getrange((creature*)object) > 0;
}
static bool is_state(const void* object) {
	return ((creature*)object)->is(last_state);
}
static bool is_weapon(const void* object) {
	return ((item*)object)->geti().isweapon();
}

static void filter_armed(int bonus) {
	opponents.match(is_armed, bonus >= 0);
}
static void filter_enemy(int bonus) {
	opponents.match(is_enemy, bonus >= 0);
}
static void filter_you(int bonus) {
	opponents.match(is_player, bonus >= 0);
}
static void filter_range(int bonus) {
	opponents.match(is_range, bonus >= 0);
}
static void filter_state(int bonus) {
	opponents.match(is_state, true);
}
static void filter_weapon(int bonus) {
	items.match(is_weapon, bonus >= 0);
}

static void select_enemies(int bonus) {
	opponents = creatures;
	opponents.match(is_enemy, bonus >= 0);
}

static void select_items(int bonus) {
	auto pb = player->wears + Backpack;
	auto pe = player->wears + BackpackLast;
	auto ps = items.begin();
	while(pb < pe) {
		if(*pb)
			*ps++ = pb;
		pb++;
	}
	items.count = ps - items.begin();
}

static bool allow_opponents(int bonus) {
	last_script->proc(bonus);
	return opponents.getcount() != 0;
}

static bool allow_items(int bonus) {
	last_script->proc(bonus);
	return items.getcount() != 0;
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

static bool if_melee_fight(int bonus) {
	return player->ismeleefight() == (bonus >= 0);
}

static bool if_choose_creature(int bonus) {
	last_script->proc(bonus);
	return (opponent != 0) == (bonus >= 0);
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

static bool if_ready_item(int bonus) {
	return player->wears[Hands].operator bool();
}
static bool if_melee_weapon(int bonus) {
	auto& ei = player->wears[Hands].geti();
	return !ei.ranged && ei.isweapon();
}
static bool if_ranged_weapon(int bonus) {
	auto& ei = player->wears[Hands].geti();
	return ei.ranged && ei.isweapon();
}
static bool if_ready_weapon(int bonus) {
	return player->wears[Hands].geti().isweapon();
}
static void ready_item(int bonus) {
	last_item = player->wears + Hands;
}

static bool if_unarmed(int bonus) {
	return !player->wears[Hands].operator bool();
}
static void unarmed(int bonus) {
	last_item = 0;
}

static bool allow_apply_state(int bonus) {
	return player->is(last_state) != (bonus >= 0);
}
static void apply_state(int bonus) {
	if(bonus>=0)
		player->states.set(last_state);
	else
		player->states.remove(last_state);
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
	auto p = getnme(sb);
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

static bool prepare_opponent() {
	opponent = opponents.choose(getnm("ChooseEnemy"));
	return opponent != 0;
}

static void make_attack(int bonus) {
	if(!prepare_opponent())
		return;
	bonus += player->getbonus(Strenght);
	player->setenemy(opponent);
	if(roll20(bonus, opponent->get(Reflex))) {
		player->actid(last_action->id);
		fix_action(false);
		script_stop();
		return;
	} else {
		if(!player->actid(last_action->id, "Miss"))
			player->actid(last_action->id);
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

static bool allow_effect(const variants& source) {
	last_wear = Backpack;
	last_item = 0;
	return script_allow(source);
}

void test_action(const char* id) {
	auto pi = bsdata<actioni>::find(id);
	if(!pi)
		return;
	for(auto v : pi->effect) {
		if(v.counter)
			an.console->addn("%1%+2i", v.getid(), v.counter);
		else
			an.console->addn(v.getid(), v.counter);
		an.console->add(script_allow(v) ? " pass" : " miss");
	}
}

static void add_actions() {
	pushvalue push_action(last_action);
	pushvalue push_wear(last_wear);
	pushvalue push_item(last_item);
	for(auto& e : bsdata<actioni>()) {
		last_action = &e;
		if(e.upgrade) {
			if(e.upgrade.iskind<feati>() && player->isfeat(e.upgrade.value))
				continue;
			else if(e.upgrade.iskind<actioni>() && answers_have(bsdata<actioni>::elements + e.upgrade.value))
				continue;
		}
		if(!allow_effect(e.effect))
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
		last_action = (actioni*)last_answer;
		last_id = last_action->id;
		script_run(last_action->effect);
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
		//test_action("AttackWithMeleeWeapon");
		make_actions();
	}
}

BSDATA(script) = {
	{"ApplyState", apply_state, allow_apply_state},
	{"FilterArmed", filter_armed, allow_opponents},
	{"FilterEnemy", filter_enemy, allow_opponents},
	{"FilterRange", filter_range, allow_opponents},
	{"FilterState", filter_state, allow_opponents},
	{"FilterYou", filter_you, allow_opponents},
	{"FilterWeapon", filter_weapon, allow_opponents},
	{"FullRoundAction", full_round_action, if_full_round_action},
	{"IfTrained", conditional_script, if_train},
	{"IfMeleeFight", conditional_script, if_melee_fight},
	{"MakeAttack", make_attack},
	{"MeleeWeapon", ready_item, if_melee_weapon},
	{"RangedWeapon", ready_item, if_ranged_weapon},
	{"ReadyItem", ready_item, if_ready_item},
	{"ReadyWeapon", ready_item, if_ready_weapon},
	{"SelectEnemies", select_enemies, allow_opponents},
	{"SelectItems", select_items, allow_items},
	{"Unarmed", unarmed, if_unarmed},
};
BSDATAF(script)