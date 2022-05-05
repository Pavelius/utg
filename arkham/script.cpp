#include "condition.h"
#include "main.h"
#include "quest.h"
#include "pathfind.h"

static ability_s	m_ability;
static locationi*	m_location;
static int			m_value;
answers				an;

static void show_text() {
	if(answers::prompt_ask) {
		an.open(getnm("Continue"), 1);
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
		if(p->index<0 || p->index>result)
			break;
		pr = p;
	}
	return pr;
}

static void apply_text(const quest* p) {
	show_text();
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

static void play() {
	apply_text(quest::last);
	play(quest::last->tags);
	show_text();
}

static void play(int n) {
	quest::last = quest::findprompt(n);
	if(quest::last)
		play();
}

static void make_roll(int bonus, int param) {
	show_text();
	auto r = game.roll(m_ability, m_value);
	quest::last = find_roll_result(quest::last, r);
	if(quest::last)
		play();
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

static void leave_street(int bonus, int param) {
}

static void encounter(int bonus, int param) {
}

static void wait_turn(int bonus, int param) {
}

static void curse(int bonus, int param) {
}

void locationi::encounter() const {
	quest::last = choose(1);
	answers::header = getnm(id);
	play();
}

BSDATA(scripti) = {
	{"Buy", make_buy},
	{"Curse", curse},
	{"Encounter", encounter},
	{"LeaveStreet", leave_street},
	{"LostInTimeAndSpace", lost_in_time_and_space},
	{"Movement", movement},
	{"Pay", make_pay},
	{"Roll", make_roll},
	{"YesNo", ask_agree},
	{"Wait", wait_turn},
};
BSDATAF(scripti)