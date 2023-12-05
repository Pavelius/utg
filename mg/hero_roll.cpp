#include "answers.h"
#include "hero.h"
#include "groupname.h"
#include "pushvalue.h"
#include "rang.h"
#include "roll.h"
#include "wise.h"

static answers			an;
static skill_s			skill;
static int				base_dices, bonus_dices, bonus_success, obstacle, opponent_dices, persona_used;
static bool				tag_nature;
static adat<char, 32>	dices;
static adat<rolli, 32>	actions, helps;

static int compare_dice_result(const void* v1, const void* v2) {
	return *((char*)v2) - *((char*)v1);
}

static int dice_result(int number) {
	auto result = 0;
	for(auto i = 0; i < dices.getcount(); i++) {
		if(dices[i] >= number)
			result++;
	}
	return result;
}

static void dice_change(int number, int new_number) {
	for(auto& e : dices) {
		if(e == number)
			e = new_number;
	}
}

static void add_dices(int count) {
	for(int i = 0; i < count; i++)
		dices.add(1 + rand() % 6);
	qsort(dices.data, dices.count, sizeof(dices.data[0]), compare_dice_result);
}

static rolli* find_help(const hero* p) {
	for(auto& e : helps) {
		if(e.player == p && (e.option == HelpDice || e.option == IAmWise))
			return &e;
	}
	return 0;
}

static rolli* find_help(rollopt_s option) {
	for(auto& e : helps) {
		if(e.option == option)
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
	auto& ei = bsdata<skilli>::elements[skill];
	for(auto i = (skill_s)0; i <= Fate; i = (skill_s)(i + 1)) {
		if(skill != i && !ei.help.is(i))
			continue;
		auto n = player->getskill(skill);
		if(n >= 0)
			return bsdata<skilli>::elements + i;
	}
	return 0;
}

static void add_skill_help() {
	for(auto p : party) {
		if(p == player)
			continue;
		if(find_help(p))
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

static bool is_allow_wise(variant subject) {
	if(environment == subject || (environment && environment->parent == subject))
		return true;
	return false;
}

static void add_wise_help() {
	for(auto p : party) {
		if(p == player)
			continue;
		if(find_help(p))
			continue;
		for(auto& e : bsdata<wisei>()) {
			if(!p->iswise(&e))
				continue;
			if(!is_allow_wise(e.subject))
				continue;
			auto pa = actions.add();
			pa->option = IAmWise;
			pa->player = p;
			pa->action = &e;
			an.add(pa, getnm("CharacterIAmWise"), p->getname(), e.getname());
		}
	}
}

static void add_persona_dices() {
	if(!player->getskill(Persona))
		return;
	if(persona_used >= 3)
		return;
	auto pa = actions.add();
	pa->option = PersonaAddDice;
	pa->player = player;
	pa->action = 0;
	an.add(pa, getnm("CharacterPersonaAddDice"));
}

static void add_persona_nature_tag() {
	if(!player->getskill(Persona))
		return;
	auto pa = actions.add();
	pa->option = PersonaAddNatureDice;
	pa->player = player;
	pa->action = 0;
	an.add(pa, getnm("CharacterPersonaAddNatureDice"), player->getskill(Nature), getnm("Dice"));
}

static void add_fate_reroll() {
	if(!player->getskill(Fate))
		return;
	if(find_help(FateSuccessReroll))
		return;
	auto pa = actions.add();
	pa->option = FateSuccessReroll;
	pa->player = player;
	pa->action = 0;
	auto count = dice_result(6);
	an.add(pa, getnm("CharacterFateReroll"), count, getnm("Dice"));
}

static void fix_roll_result() {
	player->act("[%герой] выбросил%а: ");
	auto i = 0;
	for(auto v : dices) {
		if(i)
			sb.add(", ");
		sb.adds("%1i", v);
		i++;
	}
	if(i)
		sb.add(".");
	auto result = dice_result(4);
	if(result)
		sb.adds(getnm("ScoreDiceResult"), result, getnm("Success"));
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
		switch(e.option) {
		case HelpDice:
			sb.adds(getnm("CharacterHelp"), e.player->getname());
			break;
		case IAmWise:
			sb.adds(getnm("UseIAmWise"), e.player->getname());
			break;
		}
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
		int count;
		switch(pa->option) {
		case HelpDice: case IAmWise:
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
		case DeeperUnderstanding:
			break;
		case OfCourse:
			break;
		case FateSuccessReroll:
			player->setskill(Fate, player->getskill(Fate) - 1);
			count = dice_result(6);
			while(count > 0) {
				dice_change(6, 5);
				add_dices(count);
				count = dice_result(6);
			}
			break;
		case PersonaAddDice:
			player->setskill(Persona, player->getskill(Persona) - 1);
			persona_used++;
			bonus_dices++;
			break;
		case PersonaAddNatureDice:
			player->setskill(Persona, player->getskill(Persona) - 1);
			bonus_dices += player->getskill(Nature);
			tag_nature = true;
			break;
		}
		helps.add(*pa);
	}
}

static void prepare_roll() {
	persona_used = 0;
	tag_nature = false;
	base_dices = player->getskill(skill);
	helps.clear();
	auto pb = sb.get();
	while(true) {
		an.clear(); sb.set(pb); actions.clear();
		fix_prepare_roll();
		add_skill_help();
		add_trait_help();
		add_wise_help();
		add_persona_dices();
		add_persona_nature_tag();
		auto p = an.choose(getnm("WhatDoYouDo"), getnm("MakeRoll"), 1);
		if(!p)
			break;
		resolve_action(p);
	}
	sb.set(pb);
}

static void make_roll() {
	dices.clear();
	add_dices(base_dices + bonus_dices);
	auto pb = sb.get();
	while(true) {
		an.clear(); sb.set(pb);
		fix_roll_result();
		add_fate_reroll();
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