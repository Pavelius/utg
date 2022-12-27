#include "answers.h"
#include "actable.h"
#include "crt.h"
#include "creature.h"
#include "groupname.h"

struct cactioni {
	hero*		player;
	const void*	action;
};

static answers	an;
hero			*player, *opponent;
heroa			party;
static int		base_dices, bonus_dices, bonus_success, obstacle, opponent_dices;
static adat<cactioni, 32> actions, helps;

static cactioni* find_help(const hero* p) {
	for(auto& e : helps) {
		if(e.player == p)
			return &e;
	}
	return 0;
}

static const skilli* get_help_skills(const hero* player, skill_s skill) {
	if(find_help(player))
		return 0;
	auto& ei = bsdata<skilli>::elements[skill];
	for(auto i = (skill_s)0; i <= LastSkill; i = (skill_s)(i + 1)) {
		if(skill != i && !ei.help.is(i))
			continue;
		auto n = player->get(skill);
		if(n >= 0)
			return bsdata<skilli>::elements + i;
	}
	return 0;
}

static void add_helpers(skill_s skill) {
	for(auto p : party) {
		if(p == player)
			continue;
		auto ps = get_help_skills(p, skill);
		if(!ps)
			continue;
		auto pa = actions.add();
		pa->player = p;
		pa->action = ps;
		an.add(pa, getnm("CharacterHelpRoll"), p->getname(), ps->getname());
	}
}

static void fix_roll(skill_s skill) {
	auto total_dices = base_dices + bonus_dices;
	auto pdn = getnm("Dice");
	sb.addn("[%1] ��������� ����� [%2]", player->getname(), bsdata<skilli>::elements[skill].getname());
	if(bonus_dices > 0)
		sb.adds("c ������� � [%1i] %-*2", bonus_dices, pdn);
	else if(bonus_dices < 0)
		sb.adds("c� ������� � %1i %-*2", -bonus_dices, pdn);
	if(bonus_success != 0) {
		if(bonus_dices != 0)
			sb.adds("�");
		else
			sb.adds("�");
		sb.adds("[%1i] %-*2 � ������ �����", bonus_success, getnm("Success"));
	}
	sb.add(".");
	for(auto& e : helps)
		sb.adds(getnm("CharacterHelp"), e.player->getname());
	if(total_dices == 0) {
		sb.adds("������� [%1i] %-*2", total_dices, pdn);
	} else
		sb.adds("������� [%1i] %-*2", total_dices, pdn);
	if(opponent) {
		sb.adds("��� ���� [%1] ����� ������ [%2i] %-*3",
			opponent->getname(),
			opponent_dices, pdn);
	} else if(obstacle)
		sb.adds("������ ��������� [%1i]", obstacle);
	sb.add(".");
}

void hero::roll(skill_s skill) {
	helps.clear();
	auto pb = sb.get();
	while(true) {
		actions.clear(); an.clear(); sb.set(pb);
		fix_roll(skill);
		add_helpers(skill);
		auto p = an.choose(getnm("WhatDoYouDo"), getnm("MakeRoll"), 1);
		if(!p)
			break;
		if(actions.indexof(p) != -1) {
			auto pa = (cactioni*)p;
			if(bsdata<skilli>::have(pa->action)) {
				bonus_dices++;
				helps.add(*pa);
			}
		}
	}
}

void hero::create() {
	clear();
	setname(groupname::randomid("MouseguardMale"));
}