#include "condition.h"
#include "main.h"
#include "quest.h"
#include "pathfind.h"

static ability_s	m_ability;
static locationi*	m_location;
static int			m_value;
answers				an;

static void apply_text() {
	if(answers::prompt_ask) {
		an.open(getnm("Continue"));
		answers::prompt_ask = 0;
	}
}

static const quest* find_roll_result(const quest* ph, int result) {
	const quest* pr = 0;
	auto index = ph->index;
	auto pe = bsdata<quest>::end();
	if(result > 29)
		result = 29;
	for(auto p = ph + 1; p < pe; p++) {
		if(p->index != index || p->next<0 || p->next>result)
			break;
		pr = p;
	}
	return pr;
}

static void apply_text(const quest* p) {
	apply_text();
	if(p->text)
		answers::prompt_ask = p->text;
}

static void run_script(int value, int counter) {
	auto& e = bsdata<scripti>::elements[value];
	if(e.proc)
		e.proc(counter, e.param);
}

void apply_value(variant v) {
	if(v.iskind<scripti>())
		run_script(v.value, v.counter);
	else if(v.iskind<locationi>()) {
		m_location = bsdata<locationi>::elements + v.value;
		m_value = v.counter;
	} else if(v.iskind<abilityi>()) {
		m_ability = (ability_s)v.value;
		m_value = v.counter;
	}
}

static void play(const variants& source) {
	for(auto v : source)
		apply_value(v);
}

static void play(const quest* p) {
	apply_text(p);
	play(p->tags);
}

static void make_roll(int bonus, int param) {
	apply_text();
	auto r = game.roll(m_ability, m_value);
	auto p = find_roll_result(quest::last, r);
	if(p)
		play(p);
}

static void make_pay(int bonus, int param) {
}

static void make_buy(int bonus, int param) {
}

static void ask_agree(int bonus, int param) {
}

static void lost_in_time_and_space(int bonus, int param) {
}

static void movement(int bonus, int param) {
}

void gamei::encounter(int number) {
	auto p = quest::findprompt(number);
	if(!p)
		return;
	play(p);
}

BSDATA(scripti) = {
	{"Buy", make_buy},
	{"LostInTimeAndSpace", lost_in_time_and_space},
	{"Movement", movement},
	{"Pay", make_pay},
	{"Roll", make_roll},
	{"YesNo", ask_agree},
};
BSDATAF(scripti)