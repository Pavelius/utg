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

static int d6() {
	return 1 + rand() % 6;
}

static void clear_text_manual() {
	answers::prompt = 0;
	shown_info = false;
	an.clear();
}

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

static void take_pool(int count) {
	while(count > 0) {
		an.clear();
		for(auto& e : pool.source) {
			if(!e)
				continue;
			an.add(&e, getnm(e.geti().id));
		}
		auto push_count = answers::column_count;
		answers::column_count = 1;
		auto p = (cardi*)an.choose(0);
		answers::column_count = push_count;
		if(!p)
			break;
		game.source.add(*p);
		p->clear();
		count--;
	}
	pool.discard();
	clear_text_manual();
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

static void apply_card_type(cardtype_s type, int bonus) {
	pool.pick(type, bonus);
	take_pool(bonus);
}

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
		if(m_value == 100)
			m_value = d6();
		else if(m_value == -100)
			m_value = -d6();
		else if(m_value == -101)
			m_value = -game.get(m_ability) / 2;
		else if(m_value == -102)
			m_value = -game.get(m_ability);
		if(bsdata<abilityi>::elements[m_ability].is(abilityi::Indicator))
			apply_indicator(m_ability, m_value);
	} else if(v.iskind<cardtypei>())
		apply_card_type((cardtype_s)v.value, v.counter);
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
	else if(v.iskind<abilityi>()) {
		auto nv = game.get((ability_s)v.value) + v.counter;
		if(!v.counter)
			return false;
		else if(v.counter < 0) {
			auto minimum = game.getminimal((ability_s)v.value);
			return nv >= minimum;
		} else
			return true;
	} else
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

static void apply_result_title(const char* title) {
	auto r = (int)an.choose(title);
	clear_text_manual();
	apply_result(r);
}

static void make_roll(int bonus, int param) {
	show_text();
	apply_result(game.roll(m_ability, m_value));
}

static void make_pay(int bonus, int param) {
	an.clear();
	if(game.get(Money) >= bonus)
		an.add((void*)1, getnm("PayMoney"), bonus);
	an.add((void*)0, getnm("DoNotPay"));
	auto r = (int)an.choose(0);
	clear_text_manual();
	if(r == 1)
		game.add(Money, -bonus);
	apply_result(r);
}

static void ask_agree(int bonus, int param) {
	an.clear();
	an.add((void*)1, getnm("Yes"));
	an.add((void*)0, getnm("No"));
	apply_result_title(getnm("DoYouAgree"));
}

static void lost_in_time_and_space(int bonus, int param) {
}

static void movement(int bonus, int param) {
	show_text();
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

static void nomove(int bonus, int param) {
	show_info(getnm("YouNoMove"));
}

static void choose_street_or_location(int bonus, int param) {
	for(auto& e : bsdata<locationi>()) {
		if((e.type == Street || e.type == Arkham) && &e != game.location)
			an.add(&e, getnm(e.id));
	}
	m_location = (locationi*)an.choose(getnm("ChooseStreetOrLocation"));
	m_value = 0;
	clear_text_manual();
}

static void monster_trophy(int bonus, int param) {
}

static void common_weapon(int bonus, int param) {
}

static void bless(int bonus, int param) {
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

static void take(int bonus, int param) {
	take_pool(bonus);
}

static const quest* choose_option(const char* title) {
	auto ph = quest::last;
	auto pe = bsdata<quest>::end();
	an.clear();
	for(auto p = ph + 1; p < pe; p++) {
		if(p->next == -1)
			break;
		if(!test_values(p->tags))
			continue;
		an.add(p, p->text);
	}
	auto push_count = answers::column_count;
	answers::column_count = 1;
	auto p = (quest*)an.choose(title, 0);
	answers::column_count = push_count;
	return p;
}

static void choose(int bonus, int param) {
	auto push_last = quest::last;
	char temp[128]; stringbuilder sb(temp);
	while(bonus > 0) {
		quest::last = push_last;
		sb.clear();
		sb.add(getnm("ChooseLeft"), bonus);
		auto ph = choose_option(temp);
		clear_text_manual();
		quest::last = find_entry(ph, ph->next);
		if(!quest::last)
			break;
		play();
		bonus--;
	}
}

static void gate_appear(int bonus, int param) {
}

static void gate_close(int bonus, int param) {
}

static void monster_appear(int bonus, int param) {
}

static void remove_sanity_and_gain(int bonus, int param) {
	auto n = d6();
	apply_indicator(Sanity, -n);
	apply_indicator((ability_s)param, n);
}

static void arrested(int bonus, int param) {
	show_text();
	game.movement(bsdata<locationi>::find("Prison"));
	game.add(Money, game.get(Money) / 2);
	game.delayed();
}

static void lose_half_items(int bonus, int param) {
}

static void buy_ability(int count, int cost, const char* cancel, const slice<ability_s>& source) {
	while(count--) {
		an.clear();
		for(auto v : source) {
			if(game.get(Money) >= cost)
				an.add((void*)v, getnm("PayAbility"), getnm(bsdata<abilityi>::elements[v].id), cost);
		}
		auto v = (ability_s)(int)an.choose(0, cancel);
		if(!v)
			break;
		game.add(Money, -cost);
		game.add(v, 1);
	}
}

static void heal(int bonus, int param) {
	static ability_s source[] = {Health, Sanity};
	buy_ability(bonus, param, getnm("ThatEnought"), source);
}

static void return_arkham(int bonus, int param) {
}

static void sanity_roll(int bonus, int param) {
}

static void health_roll(int bonus, int param) {
}

static void myth_location(int bonus, int param) {
	m_location = bsdata<locationi>::find("TheWitchHouse");
	m_value = 0;
}

static void play_block(int bonus, int param) {
	auto p = find_entry(quest::last, bonus);
	if(p) {
		quest::last = p;
		play();
	}
}

static void success_unique_fail_common(int bonus, int param) {
	auto successed = game.rolld6(bonus);
	pool.pick(CommonItem, bonus - successed);
	pool.pick(UniqueItem, successed);
	take_pool(bonus);
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
	{"Bless", bless},
	{"Choose", choose},
	{"ChooseStreetOrLocation", choose_street_or_location},
	{"CommonWeapon", common_weapon},
	{"Curse", curse},
	{"Delayed", delayed},
	{"Encounter", encounter},
	{"GateAppear", gate_appear},
	{"GateClose", gate_close},
	{"Heal", heal, 1},
	{"HealthRollClue", health_roll, Clue},
	{"LeaveStreet", leave_street},
	{"LoseHalfItems", lose_half_items},
	{"LostInTimeAndSpace", lost_in_time_and_space},
	{"MonsterAppear", monster_appear},
	{"MonsterTrophy", monster_trophy},
	{"Movement", movement},
	{"MovementEncounterAndBack", movement_encounter_and_back},
	{"MythLocation", myth_location},
	{"NoMove", nomove},
	{"Pay", make_pay},
	{"PayGate", pay_gate},
	{"PickCommonItem", pick_pool, CommonItem},
	{"PickSpell", pick_pool, Spell},
	{"PickUniqueItem", pick_pool, UniqueItem},
	{"Play", play_block},
	{"RemoveSanityAndGainClue", remove_sanity_and_gain, Clue},
	{"ReturnArkham", return_arkham},
	{"Roll", make_roll},
	{"SanityRollClue", sanity_roll, Clue},
	{"SuccessUniqueFailCommon", success_unique_fail_common},
	{"Take", take},
	{"Trade", trade},
	{"YesNo", ask_agree},
};
BSDATAF(scripti)