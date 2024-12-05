#include "ability.h"
#include "action.h"
#include "answers.h"
#include "area.h"
#include "condition.h"
#include "creature.h"
#include "draw.h"
#include "list.h"
#include "formula.h"
#include "modifier.h"
#include "pushvalue.h"
#include "rand.h"
#include "script.h"
#include "skill.h"
#include "stringvar.h"

static void* last_answer;
static int pure_roll, critical_roll;

static void check_bonus(int& bonus, int minimum = 1) {
	if(bonus < minimum)
		bonus = minimum;
}

static void addvalue(char& value, int bonus) {
	bonus += value;
	if(bonus > 100)
		bonus = 100;
	else if(bonus < -100)
		bonus = -100;
	value = bonus;
}

static void add_item(const itemi* pi) {
	if(!pi)
		return;
	last_item = 0;
	item it; it.create(pi);
	player->equip(it);
}

template<> void fnscript<areai>(int value, int bonus) {
	last_area_type = (arean)value;
}
template<> void fnscript<classi>(int value, int bonus) {
	last_class = (classn)value;
}
template<> void fnscript<feati>(int value, int bonus) {
	player->feats.set(value);
}
template<> void fnscript<genderi>(int value, int bonus) {
	last_gender = (gendern)value;
}
template<> void fnscript<modifieri>(int value, int bonus) {
	modifier = (modifier_s)value;
}
template<> void fnscript<weari>(int value, int bonus) {
	last_wear = (wear_s)value;
}
template<> void fnscript<skilli>(int value, int bonus) {
	last_skill = (skilln)value;
}
template<> void fnscript<statei>(int value, int bonus) {
	last_state = (staten)value;
}
template<> void fnscript<abilityi>(int value, int bonus) {
	last_ability = (abilityn)value;
	switch(modifier) {
	case Permanent: addvalue(player->basic.abilities[value], bonus); break;
	default: addvalue(player->abilities[value], bonus); break;
	}
}
template<> void fnscript<itemi>(int value, int bonus) {
	add_item((itemi*)bsdata<itemi>::elements + value);
}

bool roll20(int bonus, int dc) {
	critical_roll = 0;
	pure_roll = d20();
	last_number = pure_roll + bonus;
	auto result = last_number >= dc;
	if(pure_roll == 1) {
		critical_roll = -1;
		result = false;
	}
	if(pure_roll == 20) {
		critical_roll = 1;
		result = true;
	}
	if(result)
		answers::console->addn("[{%1i%+2i=%3i}]", pure_roll, bonus, last_number);
	else
		answers::console->addn("[-{%1i%+2i=%3i}]", pure_roll, bonus, last_number);
	return result;
}

static void print_message(int bonus) {
	switch(bonus) {
	case -1: player->act(last_id, "Fail"); break;
	case 1: player->act(last_id, "Success"); break;
	default: player->act(last_id, "Act"); break;
	}
}

static bool allow_combat() {
	int side = 0;
	for(auto p : creatures) {
		if(!p->isready())
			continue;
		auto n = p->is(Hostile) ? -1 : 1;
		if(!side)
			side = n;
		else if(side != n)
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

static void roll_skill_value(int bonus) {
	auto pi = bsdata<skilli>::elements + last_skill;
	if(player->istrain(last_skill))
		bonus += 5;
	last_number = d20() + bonus;
}

static void for_each_opponent(int bonus) {
	pushvalue push_player(opponent);
	auto elements = script_body();
	for(auto p : creatures) {
		opponent = p;
		script_run(elements);
	}
	script_stop();
}

static void for_each_player(int bonus) {
	pushvalue push_player(player);
	auto elements = script_body();
	for(auto p : creatures) {
		player = p;
		script_run(elements);
	}
	script_stop();
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

static bool if_choose_creature(int bonus) {
	last_script->proc(bonus);
	return (opponent != 0) == (bonus >= 0);
}

static void use_action(char& value, int& bonus) {
	if(!value)
		return;
	else if(value >= bonus) {
		value -= bonus;
		bonus = 0;
	} else {
		bonus -= value;
		value = 0;
	}
}

static bool if_use_standart(int bonus) {
	check_bonus(bonus);
	return player->abilities[StandartAction] >= bonus;
}
static void use_standart(int bonus) {
	check_bonus(bonus);
	player->abilities[StandartAction]--;
}

static bool if_use_move(int bonus) {
	check_bonus(bonus);
	return (player->abilities[StandartAction] + player->abilities[MoveAction]) >= bonus;
}
static void use_move(int bonus) {
	check_bonus(bonus);
	use_action(player->abilities[MoveAction], bonus);
	use_action(player->abilities[StandartAction], bonus);
}

static bool if_use_swift(int bonus) {
	check_bonus(bonus);
	return (player->abilities[StandartAction] + player->abilities[MoveAction] + player->abilities[SwiftAction]) > bonus;
}
static void use_swift(int bonus) {
	check_bonus(bonus);
	use_action(player->abilities[SwiftAction], bonus);
	use_action(player->abilities[MoveAction], bonus);
	use_action(player->abilities[StandartAction], bonus);
}

static bool if_full_round(int bonus) {
	return player->abilities[SwiftAction] > 0
		&& player->abilities[MoveAction] > 0
		&& player->abilities[StandartAction] > 0;
}
static void use_full_round(int bonus) {
	player->abilities[SwiftAction] = 0;
	player->abilities[MoveAction] = 0;
	player->abilities[StandartAction] = 0;
}

static void select_creatures(int bonus) {
	opponents = creatures;
}

static void ready_item(int bonus) {
	last_item = player->wears + Hands;
}
static bool if_ready_item(int bonus) {
	ready_item(bonus);
	return last_item->operator bool();
}
static bool if_ready_weapon(int bonus) {
	ready_item(bonus);
	return last_item->geti().isweapon();
}

static void apply_state(int bonus) {
	if(bonus >= 0)
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

static bool prepare_opponent() {
	opponent = opponents.choose(getnm("ChooseEnemy"));
	return opponent != 0;
}

static void damage(int bonus) {
	opponent->damage(bonus);
}

static void make_attack(int bonus) {
	if(!prepare_opponent())
		return;
	if(!player->wears[Hands].geti().ranged)
		bonus += player->getbonus(Strenght);
	player->setenemy(opponent);
	player->act(last_id, "Act");
	if(roll20(bonus, opponent->get(Reflex))) {
		print_message(1);
		auto damage = player->wears[Hands].geti().damage.roll();
		if(critical_roll > 0)
			damage *= 2;
		opponent->damage(damage);
	} else
		print_message(-1);
}

static bool answers_have(const void* p) {
	for(auto& e : an) {
		if(e.value == p)
			return true;
	}
	return false;
}

static void add_actions() {
	pushvalue push_action(last_action);
	pushvalue push_wear(last_wear, Backpack);
	pushvalue push_item(last_item, (item*)0);
	for(auto& e : bsdata<actioni>()) {
		last_action = &e;
		if(e.upgrade) {
			if(e.upgrade.iskind<feati>() && player->isfeat(e.upgrade.value))
				continue;
			else if(e.upgrade.iskind<actioni>() && answers_have(bsdata<actioni>::elements + e.upgrade.value))
				continue;
		}
		if(!script_allow(e.effect))
			continue;
		an.add(&e, e.getprompt());
	}
}

static void ask_answer() {
	last_answer = (void*)an.choose(getnm("WhatYouDo"));
}

static void apply_answers() {
	if(bsdata<actioni>::have(last_answer)) {
		last_action = (actioni*)last_answer;
		script_run(last_action->id, last_action->effect);
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
		if(!p->isready())
			continue;
		player = p;
		before_combat_round();
		make_actions();
	}
}

static int compare_initiative(const void* v1, const void* v2) {
	return (*((creature**)v1))->abilities[InitiativeResult] - (*((creature**)v2))->abilities[InitiativeResult];
}

static void play_combat_rounds() {
	qsort(creatures.data, creatures.count, sizeof(creatures.data[0]), compare_initiative);
	while(allow_combat())
		one_combat_round();
}

static void play_combat_rounds(int bonus) {
	draw::setnext(play_combat_rounds);
}

static void add_creature(int bonus) {
	creatures.add(player);
}

static void push_modifier(int bonus) {
	auto push = modifier;
	script_body();
	modifier = push;
}

static void set_ability(int bonus) {
	player->abilities[last_ability] = get_bonus(bonus);
}

static void set_hostile(int bonus) {
	player->abilities[Relation] = -100;
}

static bool if_train() {
	return player->istrain(last_skill);
}

static bool if_melee_fight() {
	return player->ismeleefight();
}

static bool if_item_range() {
	return last_item->geti().ranged != 0;
}

static bool if_item_reload() {
	return false;
}

static bool if_item_multi_mode() {
	return false;
}

static bool if_move_action() {
	return (player->abilities[MoveAction] + player->abilities[StandartAction]) > 0;
}

static bool if_swift_action() {
	return (player->abilities[SwiftAction] + player->abilities[MoveAction] + player->abilities[StandartAction]) > 0;
}

static bool if_state() {
	return player->states.is(last_state);
}

static bool if_wounded() {
	return player->hp < player->hpm;
}

static void print_hands(stringbuilder& sb) {
	if(!player)
		return;
	if(!player->wears[Hands])
		sb.add(getnm("Hands"));
	else
		sb.add(player->wears[Hands].getname());
}

BSDATA(stringvari) = {
	{"Hands", print_hands}
};
BSDATAF(stringvari)
BSDATA(conditioni) = {
	{"IfItemRange", if_item_range},
	{"IfItemReload", if_item_reload},
	{"IfItemMultimode", if_item_multi_mode},
	{"IfMeleeFight", if_melee_fight},
	{"IfState", if_state},
	{"IfTrained", if_train},
	{"IfWounded", if_wounded},
};
BSDATAF(conditioni)
BSDATA(script) = {
	{"AddCreature", add_creature},
	{"ApplyState", apply_state},
	{"CreateArea", create_area},
	{"CreateHero", create_hero},
	{"FilterArmed", filter_armed, allow_opponents},
	{"FilterEnemy", filter_enemy, allow_opponents},
	{"FilterRange", filter_range, allow_opponents},
	{"FilterState", filter_state, allow_opponents},
	{"FilterYou", filter_you, allow_opponents},
	{"FilterWeapon", filter_weapon, allow_opponents},
	{"ForEachPlayer", for_each_player},
	{"ForEachOpponent", for_each_opponent},
	{"MakeAttack", make_attack},
	{"PushModifier", push_modifier},
	{"PlayCombatRounds", play_combat_rounds},
	{"Print", print_message},
	{"ReadyItem", ready_item, if_ready_item},
	{"ReadyWeapon", ready_item, if_ready_weapon},
	{"RollSkillValue", roll_skill_value},
	{"SelectEnemies", select_enemies, allow_opponents},
	{"SelectItems", select_items, allow_items},
	{"SetAbility", set_ability},
	{"SetHostile", set_hostile},
	{"UseMove", use_move, if_use_move},
	{"UseStandart", use_standart, if_use_standart},
	{"UseSwift", use_swift, if_use_swift},
	{"UseFullRound", use_full_round, if_full_round},
};
BSDATAF(script)