#include "answers.h"
#include "actable.h"
#include "crt.h"
#include "creature.h"

static answers	an;
static hero*	player;
creaturea		party, helpers;
static int		total_dices, bonus_dices, bonus_success, obstacle, opponent_dices;

static bool can_help(const hero* p) {
	return helpers.indexof(p) == -1;
}

static void add_helpers(skill_s skill) {
	for(auto p : party) {
		if(!can_help(p))
			continue;
		an.add(p, getnm("CharacterHelpRoll"), p->getname());
	}
}

static void fix_roll(skill_s skill, int total_dices, int bonus_dices, int bonus_success, int obstacle, const actable* opponent, int opponent_dices) {
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
	for(auto p : helpers)
		sb.adds("%1 поможет в броске.", p->getname());
	sb.adds("Бросьте [%1i] %-*2", total_dices, pdn);
	if(opponent) {
		sb.adds("при этом [%1] будет кидать [%2i] %-*3",
			opponent->getname(),
			opponent_dices, pdn);
	} else if(obstacle)
		sb.adds("против сложности [%1i]", obstacle);
	sb.add(".");
}

void hero::roll(skill_s skill) {
	helpers.clear();
	auto pb = sb.get();
	while(true) {
		an.clear(); sb.set(pb);
		fix_roll(skill, 3, 2, 1, 3, 0, 0);
		add_helpers(skill);
		an.choose(getnm("WhatDoYouDo"), getnm("MakeRoll"), 1);
	}
}