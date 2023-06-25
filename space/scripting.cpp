#include "answers.h"
#include "ability.h"
#include "condition.h"
#include "crt.h"
#include "game.h"
#include "planet.h"
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
	if(!last_quest)
		return;
	auto index = last_quest->index;
	auto pe = bsdata<quest>::end();
	for(auto pa = last_quest + 1; pa < pe; pa++) {
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
	if(!last_quest)
		return;
	auto pn = last_quest->getheader();
	if(pn)
		answers::header = pn;
	pn = last_quest->getimage();
	if(pn)
		answers::resid = pn;
}

static void apply_text() {
	if(last_quest && last_quest->text) {
		answers::console->clear();
		answers::console->add(last_quest->text);
	}
}

static void apply_script() {
	result = 0;
	if(!last_quest)
		return;
	result = last_quest->next;
	script::run(last_quest->tags);
}

static void choose_quest_result() {
	if(!last_quest)
		return;
	auto index = last_quest->index;
	auto pe = bsdata<quest>::end();
	auto pr = last_quest + 1;
	for(auto pa = pr; pa < pe; pa++) {
		if(pa->index != index)
			continue;
		if(!pa->isanswer())
			break;
		pr = pa;
		if(result <= pr->next)
			break;
	}
	last_quest = pr;
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
	last_quest = quest::findprompt(result);
	apply_header();
	apply_text();
}

void quest::run(int index) {
	if(!answers::console)
		return;
	pushvalue push_header(answers::header);
	pushvalue push_image(answers::resid);
	pushvalue push_last(last_quest, findprompt(index));
	while(last_quest) {
		apply_header();
		apply_text();
		script::run(last_quest->tags);
		add_quest_answers();
		auto pv = an.choose(0, 0, 1);
		if(!pv)
			break;
		else if(bsdata<quest>::source.have(pv)) {
			last_quest = (quest*)pv; result = last_quest->next;
			script::run(last_quest->tags);
			last_quest = findprompt(result);
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

static void choose_move(int bonus) {
	for(auto& e : bsdata<planeti>()) {
		if(e.system != current_system)
			continue;
		an.add(&e, e.getname());
	}
	current_planet = (planeti*)an.choose(getnm("WhichWayToGo"));
}

BSDATA(script) = {
	{"ChooseMove", choose_move},
	{"Next", jump_next},
	{"PassHours", pass_hours},
	{"Roll", roll},
	{"SetVariable", set_variable},
	{"Variable", add_variable},
};
BSDATAF(script)