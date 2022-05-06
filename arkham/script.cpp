#include "condition.h"
#include "main.h"
#include "quest.h"
#include "pathfind.h"

static ability_s	m_ability;
static locationi*	m_location;
static int			m_value;
static bool			shown_info;
answers				an;
cardpool			pool;

static void trade_pool(int count, int discount, const char* cancel) {
	while(count > 0) {
		an.clear();
		for(auto& e : pool.source) {
			if(!e)
				continue;
			auto& ei = e.geti();
			auto cost = ei.getcost(discount);
			if(!cost)
				an.add(&e, getnm("TakeCard"), getnm(ei.id));
			else
				an.add(&e, getnm((cost <= game.get(Money)) ? "BuyCard" : "BuyCardNoMoney"), getnm(ei.id), cost);
		}
		auto p = (cardi*)an.choose(0, cancel, 1);
		if(!p)
			break;
		auto cost = p->geti().getcost(discount);
		if(game.get(Money) >= cost) {
			game.source.add(*p);
			game.set(Money, game.get(Money) - cost);
			p->clear();
			count--;
		}
	}
	answers::prompt = 0;
	pool.discard();
}

static void show_text() {
	if(shown_info) {
		answers::prompt = 0;
		shown_info = false;
	}
	if(answers::prompt) {
		an.choose(0, getnm("Continue"), 1);
		answers::prompt = 0;
	}
}

static void clear_text_manual() {
	answers::prompt = 0;
	shown_info = false;
}

static const quest* find_entry(const quest* ph, int result) {
	auto index = ph->index;
	auto pe = bsdata<quest>::end();
	auto prev = -1;
	for(auto p = ph + 1; p < pe; p++) {
		if(p->next != -1)
			continue;
		if(p->index == 1000 || prev >= p->index)
			break;
		if(p->index == result)
			return p;
		prev = p->index;
	}
	return 0;
}

static const quest* find_roll_result(const quest* ph, int result) {
	const quest* pr = 0;
	auto index = ph->index;
	auto pe = bsdata<quest>::end();
	if(result > 29)
		result = 29;
	auto prev = -1;
	for(auto p = ph + 1; p < pe; p++) {
		if(p->next != -1)
			continue;
		if(p->index<0 || p->index>result || p->index <= prev)
			break;
		pr = p;
		prev = p->index;
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

static void show_info(const char* format, ...) {
	char temp[260]; stringbuilder sb(temp);
	sb.addv(format, xva_start(format));
	an.clear();
	an.choose(temp, getnm("Continue"), 1);
	shown_info = true;
}

static void apply_indicator(ability_s v, int bonus) {
	if(!bonus)
		return;
	const char* format = "YouGain";
	if(bonus < 0)
		format = "YouLose";
	show_info(getnm(format), getnm(bsdata<abilityi>::elements[v].id, iabs(bonus)), iabs(bonus));
	game.add(v, bonus);
}

static void play_result(int n);

static void apply_value(variant v) {
	if(v.iskind<scripti>())
		run_script(v.value, v.counter);
	else if(v.iskind<locationi>()) {
		m_location = bsdata<locationi>::elements + v.value;
		m_value = v.counter;
	} else if(v.iskind<cardprotoi>()) {
		if(bsdata<cardprotoi>::elements[v.value].type == Ally) {
			if(bsdata<cardtypei>::elements[Ally].cards.pick(v.value)) {
				show_info(getnm("YouGainAlly"), v.getname());
				game.addcard(v.value);
			} else
				play_result(10);
		} else {
			show_info(getnm("YouGainCard"), v.getname());
			game.addcard(v.value);
		}
	} else if(v.iskind<abilityi>()) {
		m_ability = (ability_s)v.value;
		m_value = v.counter;
		if(bsdata<abilityi>::elements[m_ability].is(abilityi::Indicator))
			apply_indicator(m_ability, m_value);
	}
}

static void play(const variants& source) {
	for(auto v : source)
		apply_value(v);
}

static void play() {
	if(!quest::last)
		return;
	apply_text(quest::last);
	play(quest::last->tags);
	show_text();
}

static void play_result(int n) {
	quest::last = find_entry(quest::last, n);
	play();
}

static bool test_value(variant v) {
	if(v.iskind<cardprotoi>())
		return !game.havecard(v.value);
	else
		return true;
}

static bool test_values(const variants& source) {
	for(auto v : source) {
		if(!test_value(v))
			return false;
	}
	return true;
}

static void play(int n) {
	quest::last = quest::findprompt(n);
	play();
}

static void apply_result(int r) {
	an.clear();
	quest::last = find_roll_result(quest::last, r);
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

static void movement_encounter_and_back(int bonus, int param) {
	auto push_location = game.location;
	game.movement(m_location);
	game.encounter();
	game.movement(push_location);
}

static void leave_street(int bonus, int param) {
	show_text();
	game.leavestreet();
}

static void encounter(int bonus, int param) {
	game.location->encounter(bonus);
}

static void delayed(int bonus, int param) {
	show_info(getnm("YouDelayed"));
	game.delayed();
}

static void choose_street_or_location(int bonus, int param) {
	for(auto& e : bsdata<locationi>()) {
		if((e.type == Street || e.type == Arkham) && &e != game.location)
			an.add(&e, getnm(e.id));
	}
	m_location = (locationi*)an.choose(getnm("ChooseStreetOrLocation"));
	m_value = 0;
	an.clear();
}

static void curse(int bonus, int param) {
}

static void pick_pool(int bonus, int param) {
	pool.pick((cardtype_s)param, bonus);
}

static cardi* choose_trophy(int count) {
	for(auto& e : game.source) {
		auto& ei = e.geti();
		if(ei.type == Gate)
			an.add(&e, getnm("PayTrophy"), getnm(ei.id));
	}
	return (cardi*)an.choose(0, getnm("DoNotPay"), 1);
}

static void pay_gate(int bonus, int param) {
	auto p = choose_trophy(1);
	clear_text_manual();
	if(p) {
		p->discard();
		play_result(1);
	} else
		play_result(0);
}

static void trade(int bonus, int param) {
	trade_pool(1, bonus, getnm("ThatEnought"));
}

static const quest* choose_option(bool test_condition = false) {
	auto ph = quest::last;
	auto pe = bsdata<quest>::end();
	an.clear();
	for(auto p = ph + 1; p < pe; p++) {
		if(p->next == -1)
			break;
		if(test_condition) {
			if(!test_values(p->tags))
				continue;
		}
		an.add(p, p->text);
	}
	return (quest*)an.choose(0, 0);
}

static void choose(int bonus, int param) {
	quest::last = choose_option();
	play();
}

static void gate_appear(int bonus, int param) {
}

static void monster_appear(int bonus, int param) {
}

static int d6() {
	return 1 + rand() % 6;
}

static void remove_sanity_and_gain(int bonus, int param) {
	auto n = d6();
	apply_indicator(Sanity, -n);
	apply_indicator((ability_s)param, n);
}

static void arrested(int bonus, int param) {
	show_text();
	game.movement(locationi::find("Prison"));
	game.losehalf(Money);
	game.delayed();
}

static void lose_half_items(int bonus, int param) {
}

static void myth_location(int bonus, int param) {
	m_location = locationi::find("TheWitchHouse");
	m_value = 0;
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
	{"Choose", choose},
	{"ChooseStreetOrLocation", choose_street_or_location},
	{"Curse", curse},
	{"Delayed", delayed},
	{"Encounter", encounter},
	{"LeaveStreet", leave_street},
	{"LoseHalfItems", lose_half_items},
	{"LostInTimeAndSpace", lost_in_time_and_space},
	{"MonsterAppear", monster_appear},
	{"Movement", movement},
	{"MovementEncounterAndBack", movement_encounter_and_back},
	{"MythLocation", myth_location},
	{"GateAppear", gate_appear},
	{"Pay", make_pay},
	{"PayGate", pay_gate},
	{"PickCommonItem", pick_pool, CommonItem},
	{"PickUniqueItem", pick_pool, UniqueItem},
	{"RemoveSanityAndGainClue", remove_sanity_and_gain, Clue},
	{"Roll", make_roll},
	{"Trade", trade},
	{"YesNo", ask_agree},
};
BSDATAF(scripti)