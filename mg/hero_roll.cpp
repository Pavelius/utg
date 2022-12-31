#include "answers.h"
#include "hero.h"
#include "groupname.h"
#include "pushvalue.h"
#include "rang.h"
#include "roll.h"
#include "wise.h"

static answers	an;
static skill_s	skill;
static int		base_dices, bonus_dices, bonus_success, obstacle, opponent_dices;
static unsigned char dice_result[32] = {};
static adat<rolli, 32> actions, helps;

static void clear_dice_pool() {
	memset(dice_result, 0, sizeof(dice_result));
}

static void add_dices(int count) {
	auto n = zlen(dice_result); count += n;
	for(int i = n; i < count; i++)
		dice_result[i] = 1 + rand() % 6;
}

static rolli* find_help(const hero* p) {
	for(auto& e : helps) {
		if(e.player == p && (e.option == HelpDice || e.option == IAmWise))
			return &e;
	}
	return 0;
}

static rolli* find_help(const hero* p, const void* pa) {
	for(auto& e : helps) {
		if(e.player == p && e.action == pa)
			return &e;
	}
	return 0;
}

static rolli* find_help(const hero* p, const void* pa, rollopt_s option) {
	for(auto& e : helps) {
		if(e.player == p && e.option == option && e.action == pa)
			return &e;
	}
	return 0;
}

static const skilli* get_help_skills(const hero* player) {
	if(find_help(player))
		return 0;
	auto& ei = bsdata<skilli>::elements[skill];
	for(auto i = (skill_s)0; i <= LastSkill; i = (skill_s)(i + 1)) {
		if(skill != i && !ei.help.is(i))
			continue;
		auto n = player->getskill(skill);
		if(n >= 0)
			return bsdata<skilli>::elements + i;
	}
	return 0;
}

static void add_helpers() {
	for(auto p : party) {
		if(p == player)
			continue;
		auto ps = get_help_skills(p);
		if(!ps)
			continue;
		auto pa = actions.add();
		pa->option = HelpDice;
		pa->player = p;
		pa->action = ps;
		an.add(pa, getnm("CharacterHelpRoll"), p->getname(), ps->getname());
	}
}

static void add_trait_help() {
	for(auto i = (trait_s)0; i <= LastTraits; i = (trait_s)(i + 1)) {
		if(!player->isreadytrait(i))
			continue;
		auto ps = bsdata<traiti>::elements + i;
		if(find_help(player, ps))
			continue;
		if(!ps->advantages.is(skill))
			continue;
		auto pa = actions.add();
		pa->option = TraitsHelp;
		pa->player = player;
		pa->action = ps;
		an.add(pa, getnm("CharacterTraitsHelp"), ps->getname());
	}
}

static void fix_prepare_roll() {
	auto total_dices = base_dices + bonus_dices;
	auto pdn = getnm("Dice");
	sb.addn("[%1] тестирует навык [%2]", player->getname(), bsdata<skilli>::elements[skill].getname());
	if(bonus_dices > 0)
		sb.adds("c бонусом в [%1i] %-*2", bonus_dices, pdn);
	else if(bonus_dices < 0)
		sb.adds("cо штрафом в %1i %-*2", -bonus_dices, pdn);
	if(bonus_success != 0) {
		if(bonus_dices != 0)
			sb.adds("и");
		else
			sb.adds("с");
		sb.adds("[%1i] %-*2 в случае удачи", bonus_success, getnm("Success"));
	}
	sb.add(".");
	for(auto& e : helps) {
		if(e.option == HelpDice)
			sb.adds(getnm("CharacterHelp"), e.player->getname());
	}
	if(total_dices == 0)
		sb.adds(getnm("NoDicesForRoll"), total_dices, pdn);
	else
		sb.adds("Бросьте [%1i] %-*2", total_dices, pdn);
	if(opponent) {
		sb.adds("при этом [%1] будет кидать [%2i] %-*3",
			opponent->getname(),
			opponent_dices, pdn);
	} else if(obstacle)
		sb.adds("против сложности [%1i]", obstacle);
	sb.add(".");
}

static void resolve_action(void* p) {
	if(actions.indexof(p) != -1) {
		auto pa = (rolli*)p;
		switch(pa->option) {
		case HelpDice:
			bonus_dices++;
			break;
		case TraitsHelp:
			if(player->gettrait((trait_s)getbsi((traiti*)pa->action)) >= 3)
				bonus_success++;
			else {
				bonus_dices++;
				player->usetrait((trait_s)getbsi((traiti*)pa->action));
			}
			break;
		}
		helps.add(*pa);
	}
}

static void prepare_roll() {
	base_dices = player->getskill(skill);
	helps.clear();
	auto pb = sb.get();
	while(true) {
		an.clear(); sb.set(pb); actions.clear();
		fix_prepare_roll();
		add_helpers();
		add_trait_help();
		auto p = an.choose(getnm("WhatDoYouDo"), getnm("MakeRoll"), 1);
		if(!p)
			break;
		resolve_action(p);
	}
	sb.set(pb);
}

static void make_roll() {
	auto pb = sb.get();
	while(true) {
		an.clear(); sb.set(pb);
		auto p = an.choose(getnm("WhatDoYouDo"), getnm("ApplyResult"), 1);
		if(!p)
			break;
		resolve_action(p);
	}
	sb.set(pb);
}

void hero::roll(skill_s tested_skill) {
	pushvalue push_player(player, this);
	pushvalue push_skill(skill, tested_skill);
	prepare_roll();
	make_roll();
}