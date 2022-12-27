#include "answers.h"
#include "actable.h"
#include "crt.h"
#include "creature.h"
#include "groupname.h"
#include "pushvalue.h"
#include "rang.h"
#include "wise.h"

struct cactioni {
	hero*		player;
	const void*	action;
};

static answers	an;
hero			*player, *opponent;
heroa			party;
static int		base_dices, bonus_dices, bonus_success, obstacle, opponent_dices;
static adat<cactioni, 32> actions, helps;

static int get_rang_count(const rangi* rang) {
	auto result = 0;
	for(auto p : party) {
		if(p->getrang() == rang)
			result++;
	}
	return result;
}

static const rangi* choose_rang() {
	an.clear();
	for(auto& e : bsdata<rangi>()) {
		if(e.party_count && get_rang_count(&e) >= e.party_count)
			continue;
		an.add(&e, e.getname());
	}
	return (rangi*)an.choose(getnm("ChooseRang"));
}

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
	for(auto& e : helps)
		sb.adds(getnm("CharacterHelp"), e.player->getname());
	if(total_dices == 0) {
		sb.adds("Бросьте [%1i] %-*2", total_dices, pdn);
	} else
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

static const char* choose_options(const char* header, int count, int maximum_count) {
	static char temp[260];
	if(!maximum_count)
		return header;
	stringbuilder sb(temp);
	sb.add(header); sb.adds("(%Left [%1i]):", maximum_count - count);
	return temp;
}

static const wisei* choose_wise(const char* header, int count, int maximum_count) {
	pushvalue columns(answers::column_count, 2);
	an.clear();
	for(auto& e : bsdata<wisei>()) {
		if(player->is(&e))
			continue;
		an.add(&e, e.getname());
	}
	return (wisei*)an.choose(choose_options(header, count, maximum_count));
}

static void add_new_wises(int bonus) {
	for(auto i = 0; i < bonus; i++) {
		auto p = choose_wise(getnm("ChooseKnownWise"), i, bonus);
		player->set(p);
	}
}

static void add_player_rang() {
	auto p = choose_rang();
	player->setrang(p);
}

void hero::create() {
	pushvalue push_player(player, this);
	clear();
	setname(groupname::randomid("MouseguardMale"));
	add_player_rang();
	add_new_wises(bsdata<rangi>::elements[rang].wises);
}