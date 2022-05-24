#include "condition.h"
#include "main.h"
#include "quest.h"
#include "script.h"

// Roll results
// 1-5 Fail
// 6-9 Partial Success
// 10-12 Sure 
// 13 Critical

ability_s	last_ability;
int			last_result, last_roll, last_bonus;
static int	last_value;
static bool	need_stop;
static const char* need_prompt;

static bool allow_prompt(const variants& tags) {
	for(auto v : tags) {
		conditioni* pc = v;
		if(pc) {
			if(!pc->proc(v.counter, pc->param))
				return false;
		} else
			break;
	}
	return true;
}

static const quest* find_prompt(int index) {
	for(auto& e : bsdata<quest>()) {
		if(e.index != index)
			continue;
		if(e.isanswer())
			continue;
		if(!allow_prompt(e.tags))
			continue;
		return &e;
	}
	return 0;
}

static const quest* find_forward(int index) {
	auto pe = bsdata<quest>::end();
	for(auto pb = quest::last; pb < pe; pb++) {
		if((pb->index >= AnswersRoll && pb->index < AnswersForward)
			|| (pb->index >= AnswersForward + 100))
			break;
		if(pb->index != index)
			continue;
		if(pb->isanswer())
			continue;
		if(!allow_prompt(pb->tags))
			continue;
		return pb;
	}
	return 0;
}

static void clear_log() {
	if(answers::console)
		answers::console->clear();
}

static void add_prompt(const quest* p) {
	if(!p)
		return;
	auto id = getbsi(p);
	auto pi = getstring(id, quest::prop_header);
	if(pi)
		answers::header = pi;
	pi = getstring(id, quest::prop_image);
	if(pi)
		answers::resid = pi;
	if(answers::console && p->text)
		actv(*answers::console, p->text, 0, '\n');
}

static void add_answer(answers& an, const quest* p) {
	an.add(p, p->text);
}

static const quest* choose_answer(const quest* ph) {
	answers an;
	auto index = ph->index;
	auto pe = bsdata<quest>::end();
	for(auto p = ph + 1; p < pe; p++) {
		if(p->index != index)
			break;
		if(p->next < AnswersRoll)
			continue;
		add_answer(an, p);
	}
	if(!an)
		return 0;
	auto p = (const quest*)an.choose(0);
	need_prompt = 0;
	if(p && p->next > 0) {
		if(p->next >= AnswersForward && p->next < AnswersForward + 100)
			return find_forward(p->next);
		return find_prompt(p->next);
	}
	return 0;
}

void quest::run(int index) {
	last = find_prompt(index);
	while(last) {
		clear_log();
		need_prompt = "Continue";
		add_prompt(last);
		last = choose_answer(last);
		if(need_prompt)
			draw::pause(need_prompt);
	}
}

static void if_equal(int bonus, int param) {
	if(last_value != bonus)
		need_stop = true;
}

static void if_not_equal(int bonus, int param) {
	if(last_value == bonus)
		need_stop = true;
}

static void rolldices() {
	last_roll = 1 + rand() % 10;
	last_result = last_roll + last_bonus;
	int last_second_roll = 0;
	if(game.get(Problem) > 0) {
		game.add(Problem, -1);
		last_second_roll = 1 + rand() % 10;
		if(last_roll > last_second_roll) {
			iswap(last_second_roll, last_roll);
			last_result = last_roll + last_bonus;
		}
	}
	if(last_result < 1)
		last_result = 1;
	draw::information(getnm(last_second_roll ? "YouRollMisfortune" : "YouRoll"),
		last_result, last_roll, last_bonus, last_second_roll);
}

static void promptroll() {
	char temp[260]; stringbuilder sb(temp);
	game.set(Success, 0);
	last_bonus = game.get(Advantage);
	last_bonus += last_value;
	answers an;
	while(true) {
		sb.clear();
		sb.add(getnm("YouRollAbility"), getnm(bsdata<abilityi>::elements[last_ability].id));
		sb.adds(getnm("RollDice"));
		if(game.get(Problem))
			sb.adds("%-AddProblem");
		if(last_bonus)
			sb.adds("%-1 %+2i", getnm("AddBonus"), last_bonus);
		sb.add(".");
		an.clear();
		an.add(0, getnm("MakeRoll"));
		auto pv = an.choose(temp);
		if(!pv)
			break;
	}
	game.set(Advantage, 0);
}

static void confirmroll() {
	answers an;
	auto push_text = utg::sb.get();
	while(true) {
		rolldices();
		an.clear();
		an.add(0, getnm("ApplyRoll"));
		if(game.get(Reroll) > 0)
			an.add(&bsdata<statei>::get(Reroll), getnm("UseReroll"));
		auto pv = an.choose(0);
		if(!pv)
			break;
		if(bsdata<statei>::have(pv)) {
			switch(bsdata<statei>::source.indexof(pv)) {
			case Reroll: game.add(Reroll, -1); break;
			default: break;
			}
		}
		utg::sb.set(push_text);
	}
	last_bonus = 0;
}

static void roll(int bonus, int param) {
	promptroll();
	confirmroll();
}

static void apply(variant v) {
	if(v.iskind<scripti>())
		bsdata<scripti>::elements[v.value].apply(v.counter);
	else if(v.iskind<abilityi>()) {
		last_value = game.get((ability_s)v.value);
		last_ability = (ability_s)v.value;
	} else if(v.iskind<tagi>()) {
		// TODO: add tag or may be nothing?
	} else
		draw::warning(getnm("NotSupportType"), bsdata<varianti>::elements[v.type].id, v.value);
}

static void apply_effect(const variants& tags) {
	last_value = 0;
	need_stop = false;
	for(auto v : tags) {
		if(need_stop)
			break;
		apply(v);
	}
}

void scripti::apply(int bonus) const {
	if(proc)
		proc(bonus, param);
}

BSDATA(scripti) = {
	{"IfEqual", if_equal},
	{"IfNotEqual", if_not_equal},
	{"Roll", roll},
};
BSDATAF(scripti)