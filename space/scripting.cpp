#include "answers.h"
#include "ability.h"
#include "condition.h"
#include "crt.h"
#include "game.h"
#include "pushvalue.h"
#include "quest.h"
#include "script.h"
#include "variable.h"

static answers an;
static int result, value, variable_id, rolled[2];
static ability_s ability;

static void change_ability(ability_s v, int bonus) {
	if(!bonus)
		return;
	if(bsdata<abilityi>::elements[v].is(NegativeState)) {
		if(bonus > 0)
			draw::warning(getnm("IncreaseAbility"), bsdata<abilityi>::elements[v].getname(), bonus);
		else
			draw::information(getnm("DecreaseAbility"), bsdata<abilityi>::elements[v].getname(), -bonus);
	} else {
		if(bonus > 0)
			draw::information(getnm("IncreaseAbility"), bsdata<abilityi>::elements[v].getname(), bonus);
		else
			draw::warning(getnm("DecreaseAbility"), bsdata<abilityi>::elements[v].getname(), -bonus);
	}
	game.add(v, bonus);
}

template<> void fnscript<abilityi>(int index, int bonus) {
	ability = (ability_s)index;
	value = game.get(ability);
	change_ability(ability, bonus);
}

static void add_quest_answers() {
	an.clear();
	if(!quest::last)
		return;
	auto index = quest::last->index;
	auto pe = bsdata<quest>::end();
	for(auto pa = quest::last + 1; pa < pe; pa++) {
		if(pa->index != index)
			continue;
		if(!pa->isanswer())
			continue;
		if(!script::allow(pa->tags))
			continue;
		an.add(pa, pa->text);
	}
}

static void apply_header() {
	if(!quest::last)
		return;
	auto pn = quest::last->getheader();
	if(pn)
		answers::header = pn;
	pn = quest::last->getimage();
	if(pn)
		answers::resid = pn;
}

static void apply_text() {
	if(quest::last && quest::last->text) {
		answers::console->clear();
		answers::console->add(quest::last->text);
	}
}

static void apply_script() {
	result = 0;
	if(!quest::last)
		return;
	result = quest::last->next;
	script::run(quest::last->tags);
}

static void choose_quest_result() {
	if(!quest::last)
		return;
	auto index = quest::last->index;
	auto pe = bsdata<quest>::end();
	auto pr = quest::last + 1;
	for(auto pa = pr; pa < pe; pa++) {
		if(pa->index != index)
			continue;
		if(!pa->isanswer())
			break;
		pr = pa;
		if(result <= pr->next)
			break;
	}
	quest::last = pr;
}

static void make_roll(int bonus) {
	char temp[260]; stringbuilder sb(temp);
	if(game.get(Problem) > 0) {
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
			sb.add(getnm("YouRolledProblem"), rolled[0], rolled[1], value, result);
		else
			sb.add(getnm("YouRolled"), rolled[0], rolled[1], value, result);
		an.clear();
		if(game.get(Insight) > 0)
			an.add(bsdata<abilityi>::elements + Insight, getnm("UseInside"));
		auto p = an.choose(temp, getnm("AcceptResult"), 1);
		if(!p)
			break;
		if(bsdata<abilityi>::have(p)) {
			change_ability((ability_s)((abilityi*)p - bsdata<abilityi>::elements), -1);
			continue;
		}
	}
}

static void roll(int bonus) {
	make_roll(bonus);
	choose_quest_result();
	apply_text();
	apply_script();
	draw::pause();
	quest::last = quest::findprompt(result);
	apply_header();
	apply_text();
}

void quest::run(int index) {
	if(!answers::console)
		return;
	pushvalue push_header(answers::header);
	pushvalue push_image(answers::resid);
	pushvalue push_last(last, findprompt(index));
	while(last) {
		apply_header();
		apply_text();
		script::run(quest::last->tags);
		add_quest_answers();
		auto pv = an.choose(0, 0, 1);
		if(!pv)
			break;
		else if(bsdata<quest>::source.have(pv)) {
			last = (quest*)pv; result = last->next;
			script::run(last->tags);
			last = findprompt(result);
		}
	}
}

static void add_variable(int bonus) {
	if((int)bsdata<variable>::source.getcount() >= variable_id)
		return;
	bsdata<variable>::elements[variable_id].value += bonus;
}

static void set_variable(int bonus) {
	variable_id = bonus;
}

static void pass_hours(int bonus) {
}

static void jump_next(int bonus) {
	result = bonus;
}

BSDATA(script) = {
	{"Next", jump_next},
	{"PassHours", pass_hours},
	{"Roll", roll},
	{"SetVariable", set_variable},
	{"Variable", add_variable},
};
BSDATAF(script)