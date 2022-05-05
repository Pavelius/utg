#include "condition.h"
#include "main.h"
#include "quest.h"
#include "pathfind.h"

static ability_s	m_ability;
static locationi*	m_location;
static int			m_value;
answers				an;

static void show_text() {
	if(answers::prompt) {
		an.choose(0, getnm("Continue"), 1);
		answers::prompt = 0;
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
		answers::prompt = p->text;
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

static void apply_result(int r) {
	an.clear();
	quest::last = find_roll_result(quest::last, r);
	if(quest::last)
		play();
}

static void apply_result(const char* title) {
	auto r = (int)an.choose(title);
	answers::prompt = 0;
	apply_result(r);
}

static void make_roll(int bonus, int param) {
	show_text();
	apply_result(game.roll(m_ability, m_value));
}

static void make_pay(int bonus, int param) {
	an.clear();
	an.add((void*)1, getnm("PayMoney"), bonus);
	an.add((void*)0, getnm("DoNotPay"));
	apply_result(0);
}

static void make_buy(int bonus, int param) {
}

static void ask_agree(int bonus, int param) {
	an.clear();
	an.add((void*)1, getnm("Yes"));
	an.add((void*)0, getnm("No"));
	apply_result(getnm("DoYouAgree"));
}

static void lost_in_time_and_space(int bonus, int param) {
}

static void movement(int bonus, int param) {
	game.movement(m_location);
}

static void leave_street(int bonus, int param) {
	game.leavestreet();
}

static void encounter(int bonus, int param) {
	game.location->encounter(bonus);
}

static void delayed(int bonus, int param) {
	game.delayed();
}

static void choose_street_or_location(int bonus, int param) {
	for(auto& e : bsdata<locationi>()) {
		if((e.type == Street || e.type == Arkham) && &e!=game.location)
			an.add(&e, getnm(e.id));
	}
	m_location = (locationi*)an.choose(getnm("ChooseStreetOrLocation"));
	m_value = 0;
	an.clear();
}

static void curse(int bonus, int param) {
}

static void arrested(int bonus, int param) {
	show_text();
	game.movement(locationi::find("Prison"));
	game.losehalf(Money);
	game.delayed();
}

void locationi::encounter(int count) const {
	if(!count)
		count = 1;
	quest::last = choose(count);
	answers::header = getnm(id);
	play();
}

BSDATA(scripti) = {
	{"Arrested", arrested},
	{"Buy", make_buy},
	{"ChooseStreetOrLocation", choose_street_or_location},
	{"Curse", curse},
	{"Delayed", delayed},
	{"Encounter", encounter},
	{"LeaveStreet", leave_street},
	{"LostInTimeAndSpace", lost_in_time_and_space},
	{"Movement", movement},
	{"Pay", make_pay},
	{"Roll", make_roll},
	{"YesNo", ask_agree},
};
BSDATAF(scripti)