#include "answers.h"
#include "ability.h"
#include "condition.h"
#include "crt.h"
#include "game.h"
#include "property.h"
#include "quest.h"
#include "script.h"

static answers an;
static int counters[5], counter_id;
static int result, value, rolled[2];
static ability_s ability;

static void change_ability(ability_s v, int bonus) {
	if(!bonus)
		return;
	if(bsdata<abilityi>::elements[v].is(NegativeState)) {
		if(bonus > 0)
			draw::warning("IncreaseAbility", bsdata<abilityi>::elements[v].getname(), bonus);
		else
			draw::information("DecreaseAbility", bsdata<abilityi>::elements[v].getname(), -bonus);
	} else {
		if(bonus > 0)
			draw::information("IncreaseAbility", bsdata<abilityi>::elements[v].getname());
		else
			draw::warning("DecreaseAbility", bsdata<abilityi>::elements[v].getname(), -bonus);
	}
	game.add(ability, bonus);
}

template<> void fnscript<abilityi>(int index, int bonus) {
	ability = (ability_s)index;
	value = game.get(ability);
	change_ability(ability, bonus);
}

static void roll(int bonus) {
	char temp[260]; stringbuilder sb(temp);
	if(game.get(Problem)) {
		change_ability(Problem, -1);
		rolled[1] = 1 + (rand() % 12);
	} else
		rolled[1] = 0;
	while(true) {
		rolled[0] = 1 + (rand() % 12);
		if(rolled[1] && rolled[0] > rolled[1])
			iswap(rolled[0], rolled[1]);
		result = rolled[0] + value;
		sb.clear();
		if(rolled[1])
			sb.add("YouRolledProblem", rolled[0], rolled[1], value, result);
		else
			sb.add("YouRolled", rolled[0], rolled[1], value, result);
		an.clear();
		if(game.get(Inside) > 0)
			an.add(bsdata<abilityi>::elements + Inside, getnm("UseInside"));
		auto p = an.choose(temp, getnm("AcceptResult"), 1);
		if(!p)
			break;
		if(bsdata<abilityi>::have(p)) {
			change_ability((ability_s)((abilityi*)p - bsdata<abilityi>::elements), -1);
			continue;
		}
	}
}

void quest::run(int index) {
	if(!answers::console)
		return;
	auto push_last = last;
	last = findprompt(index);
	while(last) {
		an.clear();
		answers::console->addn(last->text);
		script::run(last->tags);
		auto index = last->index;
		auto pe = bsdata<quest>::end();
		for(auto pa = last + 1; pa < pe; pa++) {
			if(pa->index != index)
				continue;
			if(!pa->isanswer())
				continue;
			if(!script::allow(pa->tags))
				continue;
			an.add(pa, pa->text);
		}
		auto pv = an.choose(0, 0, 1);
		if(!pv)
			break;
		if(bsdata<quest>::source.have(pv)) {
			last = (quest*)pv;
			script::run(last->tags);
			last = findprompt(last->next);
		}
		answers::console->clear();
	}
	draw::pause();
}

static void add_counter(int bonus) {
	counters[counter_id] += bonus;
}

static void set_counter(int bonus) {
	counter_id = bonus;
}

static void pass_hours(int bonus) {
}

BSDATA(script) = {
	{"Counter", add_counter},
	{"PassHours", pass_hours},
	{"Roll", roll},
	{"SetCounter", set_counter},
};
BSDATAF(script)