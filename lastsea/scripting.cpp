#include "condition.h"
#include "counters.h"
#include "main.h"
#include "property.h"
#include "pathfind.h"
#include "pushvalue.h"

gamei game;
static int last_counter, last_value, last_action;
static char round_skills_bonus[Navigation + 1];
static bool	need_sail, need_stop, need_stop_actions;
static tilei* last_tile;
static int game_result;
static abilityn last_ability;
static int prop_end_scene, prop_visit, prop_maximum_danger;
static const quest* last_location;
static const quest* new_location;
counters variables;

void print(stringbuilder& sb, const variants& source);
void main_menu();

void gamei::stop(const char* format) {
	if(!utg::sb)
		return;
	if(!format)
		format = getnm("Continue");
	pause(format);
}

static void add_header(const quest* ph) {
	auto pv = ph->getheader();
	if(pv)
		answers::header = pv;
	pv = ph->getimage();
	if(pv)
		answers::resid = pv;
}

static void add_answer(answers& an, const quest* p) {
	char temp[260]; stringbuilder sb(temp);
	sb.add(p->text);
	if(p->is(bsdata<tagi>::elements + VisitManyTimes))
		sb.adds(getnm(bsdata<tagi>::elements[VisitManyTimes].id));
	an.add(p, temp);
}

void gamei::apply(const variants& tags) {
	auto push_stop = need_stop;
	need_stop = false;
	last_value = 0;
	for(auto v : tags) {
		if(need_stop)
			break;
		game.apply(v);
	}
	need_stop = push_stop;
}

static bool allow_promt(const quest* p, const variants& tags) {
	auto push_last = last_quest;
	last_quest = p;
	for(auto v : tags) {
		conditioni* pc = v;
		if(pc) {
			if(!pc->proc())
				return false;
			continue;
		}
		break;
	}
	last_quest = push_last;
	return true;
}

const quest* find_promt(int index) {
	for(auto& e : bsdata<quest>()) {
		if(e.next != -1)
			continue;
		if(e.index != index)
			continue;
		if(!allow_promt(&e, e.tags))
			continue;
		return &e;
	}
	return 0;
}

static const quest* find_forward(int bonus) {
	auto pe = bsdata<quest>::end();
	auto index = AnswerForward + bonus;
	for(auto p = last_quest + 1; p < pe; p++) {
		if(p->next != -1)
			continue;
		if(p->index < AnswerForward || p->index >= AnswerForward + 100)
			break;
		if(p->index == index)
			return p;
	}
	return 0;
}

static const quest* apply_answers(const quest* ph) {
	if(!ph)
		return 0;
	answers an;
	auto index = ph->index;
	auto pe = bsdata<quest>::end();
	for(auto p = ph + 1; p < pe; p++) {
		if(p->index != index)
			break;
		if(p->next < AnswerPage)
			continue;
		add_answer(an, p);
	}
	if(!an)
		return 0;
	auto p = (const quest*)an.choose(0);
	if(p && p->next > 0) {
		if(p->next >= AnswerForward && p->next < AnswerForward + 1000)
			return find_forward(p->next - AnswerForward);
		return find_promt(p->next);
	}
	return 0;
}

static bool apply_choose(const quest* ph, const char* title, int count, const char* cancel);

static const char* find_separator(const char* pb) {
	auto p = pb;
	while(*p) {
		if(*p == '-' && p[1] == '-' && p[2] == '-' && (p[3] == 10 || p[3] == 13) && p > pb && (p[-1] == 10 || p[-1] == 13))
			return p;
		p++;
	}
	return 0;
}

void gamei::information(const char* format, ...) {
	utg::sb.addn("[+");
	utg::sb.addv(format, xva_start(format));
	utg::sb.add("]");
}

void gamei::warning(const char* format, ...) {
	utg::sb.addn("[-");
	utg::sb.addv(format, xva_start(format));
	utg::sb.add("]");
}

static void add_message(const char* format) {
	if(!format)
		return;
	utg::sb.clear();
	while(true) {
		auto p = find_separator(format);
		if(!p)
			break;
		auto pn = skipspcr(p + 3);
		while(p < format && (p[-1] == 10 || p[-1] == 13))
			p--;
		char temp[4096];
		auto count = p - format;
		if(count > sizeof(temp) / sizeof(temp[0]) - 1)
			count = sizeof(temp) / sizeof(temp[0]) - 1;
		memcpy(temp, format, count);
		temp[count] = 0;
		game.act(temp);
		pause();
		utg::sb.clear();
		format = pn;
	}
	game.act(format);
}

static void apply_effect(const quest* p) {
	while(p) {
		last_quest = p;
		add_header(p);
		if(p->text && p->next != AnswerChoose)
			add_message(p->text);
		game.apply(p->tags);
		p = apply_answers(p);
	}
}

static bool allow_choose(abilityn v, int bonus) {
	switch(v) {
	case Infamy: case Discontent: case Danger:
		return true;
	default:
		return (game.get(v) + bonus) >= 0;
	}
}

static bool allow_choose_script(const scripti& e, int counter) {
	if(e.choose)
		return e.choose(counter, e.param);
	return true;
}

static bool allow_choose(variant v) {
	switch(v.type) {
	case Ability: return allow_choose((abilityn)v.value, v.counter);
	case Script: return allow_choose_script(bsdata<scripti>::get(v.value), v.counter);
	default: return true;
	}
}

static bool allow_choose(const quest* p) {
	for(auto v : p->tags) {
		if(!allow_choose(v))
			return false;
	}
	return true;
}

static void add_treasure(answers& an, const treasurei* p, const variants& use) {
	char temp[260]; stringbuilder sb(temp);
	print(sb, use);
	an.add(p, getnm("UseTreasureToGain"), getnm(p->id), temp);
}

static void use_treasure(const void* object) {
	if(bsdata<treasurei>::have(object))
		((treasurei*)object)->triggered();
}

static void use_treasure(void* object, useda& used) {
	if(bsdata<treasurei>::have(object)) {
		used.add(object);
		((treasurei*)object)->triggered();
	}
}

static bool apply_choose(const quest* ph, const char* title, int count, const char* cancel = 0) {
	struct handler : chooselist {
		void apply(int index, const void* object) override {
			use_treasure(object);
			if(bsdata<quest>::have(object))
				apply_effect((quest*)object);
		}
		bool isallow(int index, const void* object) const override {
			auto p = (quest*)object;
			if(!allow_choose(p))
				return false;
			if(p->is(bsdata<tagi>::elements + VisitManyTimes))
				return true;
			if(ismarked(index))
				return false;
			return true;
		}
	};
	if(!count)
		count = last_value;
	if(count > 10)
		count = 10;
	else if(count < 0)
		count = 0;
	if(!ph || !count)
		return false;
	handler san;
	auto index = ph->index;
	auto pe = bsdata<quest>::end();
	for(auto p = ph + 1; p < pe; p++) {
		if(p->index != index)
			break;
		if(p->next != AnswerChoose)
			continue;
		add_answer(san, p);
	}
	if(index >= AnswerStandartActions) {
		auto need_bonus = index - AnswerStandartActions;
		for(auto& e : bsdata<treasurei>()) {
			if(!e.isactive() || e.isdiscarded())
				continue;
			if(e.trigger != WhenChooseSpecial)
				continue;
			if(e.bonus != need_bonus)
				continue;
			add_treasure(san, &e, e.use);
		}
	}
	return san.choose(title, count, cancel);
}

static void apply_choose(int page, int count) {
	auto p = find_promt(page);
	if(p) {
		auto push_quest = last_quest;
		apply_choose(p, p->text, count);
		last_quest = push_quest;
	}
}

static bool apply_choose(int page, int count, const char* cancel) {
	auto r = false;
	auto p = find_promt(page);
	if(p) {
		auto push_last = last_quest;
		r = apply_choose(p, p->text, count, cancel);
		last_quest = push_last;
	}
	return r;
}

static const quest* find_roll_result(const quest* ph, int result) {
	const quest* pr = 0;
	auto index = ph->index;
	auto pe = bsdata<quest>::end();
	if(result > 29)
		result = 29;
	for(auto p = ph + 1; p < pe; p++) {
		if(p->index != index || p->next<1 || p->next > result)
			break;
		pr = p;
	}
	return pr;
}

static void apply_roll_result(const quest* ph, int value) {
	apply_effect(find_roll_result(ph, value));
}

static const quest* find_action(int n) {
	if(!last_location)
		return 0;
	auto parent_index = last_location->index;
	auto pe = bsdata<quest>::end();
	auto index = 0;
	for(auto p = last_location + 1; p < pe; p++) {
		if(p->index != parent_index)
			break;
		if(index++ == n)
			return p;
	}
	return 0;
}

static bool allow_action(abilityn v, int bonus) {
	switch(v) {
	case Mission: case Cabine:
		return (game.get(v) + bonus) <= game.getmaximum(v);
	default: return true;
	}
}

static bool allow_action(variant v) {
	switch(v.type) {
	case Ability: return allow_action((abilityn)v.value, v.counter);
	default: return true;
	}
}

static bool allow_action(const quest* p) {
	for(auto v : p->tags) {
		if(!allow_action(v))
			return false;
	}
	return true;
}

static void choose_actions(int count) {
	struct handler : chooselist {
		void apply(int index, const void* object) override {
			auto n = (quest*)object - bsdata<quest>::elements;
			game.addaction(n);
		}
		bool isallow(int index, const void* object) const override {
			auto p = (quest*)object;
			if(!allow_action(p))
				return false;
			if(game.islocked(index))
				return false;
			if(p->is(bsdata<tagi>::elements + VisitManyTimes))
				return true;
			if(ismarked(index))
				return false;
			return true;
		}
	};
	if(!last_location)
		return;
	last_quest = last_location;
	add_header(last_quest);
	if(last_quest->text)
		add_message(last_quest->text);
	game.apply(last_quest->tags);
	game.clearactions();
	handler san;
	auto index = last_quest->index;
	auto pe = bsdata<quest>::end();
	for(auto p = last_quest + 1; p < pe; p++) {
		if(p->index != index)
			break;
		if(p->is(bsdata<tagi>::elements + VisitRequired) && count > 0) {
			if(!p->is(bsdata<tagi>::elements + NotUseAction))
				count--;
			san.apply(0, p);
			continue;
		}
		add_answer(san, p);
	}
	san.choose(getnm("WhatDoYouWantToVisit"), count);
	game.sortactions();
}

static void add_treasure(answers& an, const treasurei* p) {
	an.add(p, "%Use [%1].", getnm(p->id));
}

static void apply_treasure(trigger_s trigger, abilityn ability) {
	for(auto& e : bsdata<treasurei>()) {
		if(!e.isactive() || e.isdiscarded())
			continue;
		if(e.trigger != trigger || e.ability!=ability)
			continue;
		e.apply();
	}
}

static void add_treasure(answers& an, trigger_s trigger) {
	for(auto& e : bsdata<treasurei>()) {
		if(!e.isactive() || e.isdiscarded())
			continue;
		if(e.trigger != trigger)
			continue;
		add_treasure(an, &e);
	}
}

static void add_treasure(answers& an, trigger_s trigger, abilityn ability, useda& used) {
	for(auto& e : bsdata<treasurei>()) {
		if(!e.isactive() || e.isdiscarded())
			continue;
		if(e.trigger != trigger)
			continue;
		if(e.ability != ability)
			continue;
		if(used.is(&e))
			continue;
		an.add(&e, getnm("UseTreasureToGainBonus"), getnm(e.id), e.bonus);
	}
}

static void summary_action(const char* id, trigger_s trigger) {
	useda used;
	while(!draw::isnext()) {
		answers an;
		an.add(0, getnm(id));
		add_treasure(an, trigger);
		auto p = an.choose();
		if(!p)
			break;
		use_treasure(p, used);
	}
}

static void play_actions() {
	need_stop_actions = false;
	last_action = 0;
	game.shuffleparcipant();
	auto parcipant_index = 0;
	for(auto v : game.actions) {
		last_action++;
		if(!v)
			continue;
		auto p = bsdata<quest>::elements + v;
		if(!p->next)
			continue;
		game.setorder(parcipant_index);
		game.script(p->next);
		if(need_stop_actions)
			break;
		summary_action("NextAction", WhenUse);
		parcipant_index++;
	}
}

void start_scene() {
	auto need_continue = false;
	if(new_location) {
		need_continue = true;
		last_location = new_location;
		new_location = 0;
		memset(round_skills_bonus, 0, sizeof(round_skills_bonus));
		choose_actions(player_count);
		play_actions();
		auto pn = getnumber(getbsi(last_location), prop_end_scene);
		if(pn)
			game.script(pn);
		if(!new_location)
			new_location = last_location;
	}
	if(need_sail && last_tile) {
		need_continue = true;
		need_sail = false;
		game.script(last_tile->param);
	}
	if(need_continue) {
		if(!draw::isnext())
			draw::setnext(start_scene);
	}
}

static tilei* sail_next_hexagon() {
	auto index = game.oceani::chooseroute(0, 1);
	if(index == pathfind::Blocked)
		return 0;
	game.setmarker(index);
	auto p = tilei::findindex(index);
	if(!p) {
		p = tilei::pick();
		if(!p)
			return 0;
		p->setindex(index);
		game.createobjects();
		game.showsplash();
	}
	need_sail = true;
	return p;
}

int	gamei::getmaximumeat() const {
	return (get(Crew) + 1) / 2;
}

int gamei::getpage() {
	if(!last_location)
		return 0;
	return last_location->index;
}

static void generate_classes() {
	auto push_interactive = answers::interactive;
	answers::interactive = false;
	for(auto& e : bsdata<pirate>())
		e.generate();
	answers::interactive = push_interactive;
}

void pirate::makeroll(int mode) {
	char temp[260]; stringbuilder sb(temp);
	last_bonus = 0;
	if(last_ability >= Exploration && last_ability <= Navigation) {
		last_bonus += round_skills_bonus[last_ability];
		last_bonus += last_value;
		last_bonus += getbonus(last_ability);
	}
	const int GunBonus = 10;
	static bool gun_used;
	answers an; useda used;
	gun_used = false;
	auto rolled_ability = (abilityn)last_ability;
	while(true) {
		sb.clear();
		if(last_ability >= Exploration && last_ability <= Navigation)
			sb.add(getnm("YouRollAbility"), getnm(bsdata<abilityi>::elements[last_ability].id));
		sb.adds(getnm("RollDice"));
		if(get(Misfortune))
			sb.adds("%-AddMisfortune");
		if(last_bonus)
			sb.adds("%-1 %+2i", getnm("AddBonus"), last_bonus);
		sb.add(".");
		an.clear();
		an.add(0, getnm("MakeRoll"));
		if(mode == 1 && !gun_used) {
			for(auto level = 1; level <= 4; level++) {
				if(!game.cannoneer::is(level, true))
					continue;
				auto bonus = game.getgunbonus(level);
				if(bonus)
					an.add((void*)(GunBonus + level), getnm("UseGun"), level, bonus);
			}
		}
		add_treasure(an, WhenRoll, last_ability, used);
		auto pv = an.choose(temp);
		if(!pv)
			break;
		else if((int)pv >= GunBonus && (int)pv <= GunBonus + 4) {
			auto level = (int)pv - GunBonus;
			last_bonus += game.getgunbonus(level);
			gun_used = game.unloadgun(level, true);
		}
		use_treasure(pv, used);
	}
	apply_treasure(WhenRollSuccess, rolled_ability);
}

void pirate::roll(int mode) {
	makeroll(mode);
	confirmroll();
	apply_roll_result(last_quest, last_result);
}

void gamei::script(int page) {
	apply_effect(find_promt(page));
}

void gamei::clear() {
	memset(this, 0, sizeof(*this));
	shiplog::clear();
}

void gamei::createtreasure() {
	treasurei::prepare();
}

static void return_all_tiles() {
	for(auto& e : bsdata<tilei>()) {
		e.tags.remove(Discarded);
		e.index = 0xFFFF;
	}
}

static void discard_unused_tiles(int from, int to) {
	for(auto& e : bsdata<tilei>()) {
		if(e.param >= from && e.param <= to)
			continue;
		if(e.isactive())
			continue;
		e.tags.set(Discarded);
	}
}

static void remove_active_tiles(int from, int to) {
	for(auto& e : bsdata<tilei>()) {
		if(e.isactive() && e.param >= from && e.param <= to)
			e.index = 0xFFFF;
	}
}

void gamei::createtiles() {
	return_all_tiles();
}

void gamei::chartacourse(int count) {
	while(count > 0) {
		char temp[260]; stringbuilder sb(temp);
		sb.add(getnm("PlaceNavigationTile"), count);
		createobjects();
		showsplash();
		addpossiblecourse();
		showsplash();
		auto index = choose(temp);
		auto p = tilei::pick();
		if(!p)
			break;
		p->setindex(index);
		count--;
	}
	createobjects();
	showsplash();
}

void gamei::showseamap() {
	game.oceani::showseamap();
}

bool gamei::ischoosed(int i) const {
	for(auto v : actions) {
		if(v == i)
			return true;
	}
	return false;
}

static void ability_command(abilityn v, int bonus) {
	last_ability = v;
	if(bonus) {
		game.add(last_ability, bonus);
		last_value = game.get(last_ability);
	}
}

static void script_command(int type, int bonus) {
	auto& ei = bsdata<scripti>::elements[type];
	ei.proc(bonus, ei.param);
}

void gamei::apply(variant v) {
	switch(v.type) {
	case Ability: ability_command((abilityn)v.value, v.counter); break;
	case Card: game.gaintreasure((treasurei*)v.getpointer()); break;
	case Script: script_command(v.value, v.counter); break;
	}
}

static bool treasure_active(const treasurei* p) {
	return p && p->isactive() && !p->isdiscarded();
}

static bool if_eat_supply(int counter, int param) {
	return game.get((abilityn)param) >= game.getmaximumeat();
}

static bool if_tag(int counter, int param) {
	return game.istag(param + counter);
}

static bool if_story(int counter, int param) {
	return treasure_active(treasurei::pickstory(counter));
}

static bool if_treasure(int counter, int param) {
	return treasure_active(treasurei::pickvaluable(counter));
}

static bool if_visit(int counter, int param) {
	return last_quest && (getnumber(last_quest->index, prop_visit) == counter);
}

static void bury(int bonus, int param) {
	game.bury(bonus);
}

static void end_game(int bonus, int param) {
	game_result = param;
	if(!param)
		pause(getnm("LostGame"));
	else {
		pause(getnm("WinGame"));
		auto history = game.get(History);
		if(history >= 5)
			game.epilog(3);
		else if(history >= 4)
			game.epilog(2);
		else
			game.epilog(1);
	}
	draw::setnext(main_menu);
}

static void run_script(int bonus, int param) {
	game.script(param + bonus);
}

static void set_value(int bonus, int param) {
	if(bonus >= 0)
		last_value = param + bonus;
	else
		last_value = param - bonus;
}

static void set_ability(int bonus, int param) {
	last_value = bonus;
	game.set((abilityn)param, bonus);
}

static void change_scene(int bonus, int param) {
	auto p = find_promt(4000 + bonus);
	if(bonus && last_location != p) {
		new_location = p;
		game.unlockall();
		variables.clear();
	}
	summary_action("NextScene", WhenUse);
}

static void mark_entry(int bonus, int param) {
	if(bonus >= 0) {
		auto i = param + bonus;
		auto pn = game.getentryname(i);
		if(pn)
			game.information(getnm("AddEntry"), pn);
		game.settag(i);
	} else {
		auto i = param - bonus;
		auto pn = game.getentryname(i);
		if(pn)
			game.information(getnm("RemoveEntry"), pn);
		game.removetag(i);
	}
}

static void apply_tile(int bonus, int param) {
	set_value(bonus, param);
	if(bonus < 0) {
		for(auto& e : bsdata<tilei>()) {
			if(e.isactive() && e.param == last_value)
				e.discard();
		}
	}
}

static void block_action(int bonus, int param) {
	if(bonus > 0)
		game.lock(bonus - 1);
	else if(bonus < 0)
		game.unlock(bonus - 1);
}

static void choose_case(int bonus, int param) {
	apply_choose(last_quest, 0, bonus);
}

static void make_roll(int bonus, int param) {
	game.roll(param);
}

static void make_roll_silent(int bonus, int param) {
	rollv(bonus);
	apply_roll_result(last_quest, last_result);
}

static void damage(int bonus, int param) {
	if(!bonus)
		bonus = last_value;
	if(bonus)
		apply_choose(6200, bonus);
	else
		pause();
}

static void add_round_bonus(int bonus, int param) {
	if(param == -1) {
		for(auto& e : round_skills_bonus)
			e += bonus;
	} else if((unsigned)param < sizeof(round_skills_bonus) / sizeof(round_skills_bonus[0]))
		round_skills_bonus[param] += bonus;
}

static void add_skill(int bonus, int param) {
	game.raiseskills(bonus);
}

static void scout_area(int bonus, int param) {
	game.chartacourse(bonus);
}

static void set_ability_if_not_info(stringbuilder& sb, const scripti& e, int bonus) {
	sb.add(getnm("SetAbilityIfNot"), getnm(bsdata<abilityi>::get(e.param).id), bonus);
}

static void set_ability_if_not(int bonus, int param) {
	if(game.get((abilityn)param) > bonus) {
		game.set((abilityn)param, bonus);
		need_stop = true;
	}
}

static void check_danger(int bonus, int param) {
	auto m = game.getmaximum(Danger);
	auto v = game.get(Danger);
	if(v >= m) {
		game.add(Danger, -100);
		game.add(Threat, 1);
	}
}

static void minus_counter(int bonus, int param) {
	last_value -= variables.get(param);
}

static void set_counter(int bonus, int param) {
	if(param != -1)
		last_counter = param;
	last_value = variables.get(last_counter);
	abilityi::correct(last_value, bonus, 0, 14);
	if(bonus) {
		last_value += bonus;
		auto pn = variables.getname(last_counter);
		if(pn)
			game.information("%1%+2i", pn, bonus);
		variables.set(last_counter, last_value);
	}
}

static void if_equal(int bonus, int param) {
	if(last_value != bonus)
		need_stop = true;
}

static void apply_forward(int bonus) {
	auto p = find_forward(bonus);
	if(p) {
		apply_effect(p);
		need_stop = true;
	}
}

static void if_zero_forward(int bonus, int param) {
	if(last_value == 0)
		apply_forward(bonus);
}

static void if_non_zero_forward(int bonus, int param) {
	if(last_value != 0)
		apply_forward(bonus);
}

static void if_not_sail(int bonus, int param) {
	if(need_sail)
		need_stop = true;
}

static void if_choosed_action(int bonus, int param) {
	if(bonus > 0) {
		if(!game.ischoosed(bsdata<quest>::source.indexof(find_action(bonus - 1))))
			need_stop = true;
	} else if(bonus < 0) {
		if(game.ischoosed(bsdata<quest>::source.indexof(find_action(-bonus - 1))))
			need_stop = true;
	}
}

static void if_last(int bonus, int param) {
	const auto m = sizeof(game.actions) / sizeof(game.actions[0]);
	if(last_action >= m - 1)
		return;
	if(game.actions[last_action] == game.actions[last_action + 1])
		need_stop = true;
}

static void set_ship(int bonus, int param) {
	game.setmarker(bonus);
}

static void add_tile(int bonus, int param) {
	last_tile = tilei::pick(last_value);
	if(last_tile)
		last_tile->index = bonus;
}

static void remove_tile(int bonus, int param) {
	last_tile = tilei::find(last_value);
	if(last_tile)
		last_tile->index = 0xFFFF;
}

static void tile_move_to_player(int bonus, int param) {
	last_tile = tilei::find(last_value);
	if(!last_tile)
		return;
	game.stop(0);
	last_tile->moveto(game.getmarker(), bonus);
	if(last_tile->index == game.getmarker())
		game.script(last_tile->param);
}

static void show_map(int bonus, int param) {
	oceani::createobjects();
	oceani::showsplash();
}

static void sail_away(int bonus, int param) {
	if(bonus)
		utg::sb.clear();
	game.stop(getnm("SailAway"));
	last_tile = sail_next_hexagon();
}

static void zero_counters(int bonus, int param) {
	variables.clear();
}

static void page_forward(int bonus, int param) {
	if(param)
		pause();
	apply_effect(find_forward(bonus));
}

static bool choose_add_gun(int bonus, int param) {
	return game.addgun(bonus, true, false);
}

static void remove_gun(int bonus, int param) {
}

static void add_gun(int bonus, int param) {
	if(game.addgun(bonus, param, true))
		game.information(getnm(param ? "GunAdded" : "GunAddedUnloaded"), bonus);
}

static void add_treasure(int bonus, int param) {
	game.gaintreasures(bonus);
}

static void upgrade_gun(int bonus, int param) {
}

static bool choose_reload_gun(int bonus, int param) {
	if(bonus >= 0)
		return game.reloadgun(4, false);
	else
		return game.unloadgun(4, false);
}

static void reload_gun_or_add(int bonus, int param) {
	if(bonus >= 0) {
		if(game.reloadgun(4, true))
			game.information(getnm("GunReloaded"));
		else if(param)
			game.add((abilityn)param, 1);
	} else {
		if(game.unloadgun(4, true))
			game.warning(getnm("GunUnloaded"));
		else if(param)
			game.add((abilityn)param, -1);
	}
}

static void full_throttle(int bonus, int param) {
	auto cancel = getnm("StopMoveShip");
	while(!draw::isnext()) {
		if(bonus == 0) {
			if(!apply_choose(AnswerCustom + 3, 3, cancel))
				return;
			last_tile = sail_next_hexagon();
			break;
		} else if(bonus == 1) {
			if(!apply_choose(AnswerCustom + 3, 2, cancel))
				return;
			last_tile = sail_next_hexagon();
		} else {
			if(!apply_choose(AnswerCustom + 6, 1, cancel))
				return;
			last_tile = sail_next_hexagon();
		}
		if(last_tile && last_tile->is(StopMovement))
			break;
	}
}

static void trade_friend(int bonus, int param) {
	game.tradefriend();
}

static void play_stars(int bonus, int param) {
	game.levelup();
}

static void entry(int bonus, int param) {
	last_value = game.istag(param + bonus) ? 1 : 0;
}

static void counter_name(int bonus, int param) {
	variables.setname(bonus, quest_text(last_value));
}

static bool is_enemy_counter(int i) {
	switch(i) {
	case 0: case 1: case 4: return true;
	default: return false;
	}
}

static void choose_counter(int bonus, int param) {
	answers an;
	for(auto i = 0; i < variables.getcount(); i++) {
		auto enemy = is_enemy_counter(i) ? 1 : 0;
		if(enemy != param)
			continue;
		auto pn = variables.getname(i);
		if(!pn)
			continue;
		an.add((void*)i, pn);
	}
	last_counter = (int)an.choose(getnm("ChooseTarget"));
}

static void stop_actions(int bonus, int param) {
	need_stop_actions = true;
}

static void choose_custom(int bonus, int param) {
	apply_choose(param + bonus, 1);
}

static void mark_visit(int bonus, int param) {
	if(!bonus)
		bonus = 1;
	addnumber(last_quest->index, prop_visit, bonus);
}

static void set_visit(int bonus, int param) {
	if(!bonus)
		removenumber(last_quest->index, prop_visit);
	else
		setproperty(last_quest->index, prop_visit, bonus);
}

static void remove_all_navigation(int bonus, int param) {
	remove_active_tiles(1, 30);
}

static void eat_supply(int bonus, int param) {
	game.add((abilityn)param, -(game.getmaximumeat() + bonus));
}

void print(stringbuilder& sb, const char* id, int count, unsigned flags, char sep = ' ');

static void eat_supply_info(stringbuilder& sb, const scripti& e, int bonus) {
	print(sb, getnm(bsdata<abilityi>::elements[e.param].id), -(game.getmaximumeat() + bonus), e.flags);
}

static void set_goal(int bonus, int param) {
	game.setgoal(bonus);
}

#ifdef _DEBUG
static void test_correction() {
	int bonus;
	bonus = -2; abilityi::correct(1, bonus, 0, 10);
	bonus = -4; abilityi::correct(1, bonus, 0, 10);
	bonus = -2; abilityi::correct(4, bonus, 0, 10);
	bonus = 3; abilityi::correct(9, bonus, 0, 10);
}
#endif // _DEBUG

void initialize_script() {
#ifdef _DEBUG
	test_correction();
#endif // _DEBUG
	// Properties
	prop_end_scene = add_property("EndScene", propertyi::Number);
	prop_visit = add_property("Visit", propertyi::Number);
	prop_maximum_danger = add_property("MaximumDanger", propertyi::Number);
	// Prompt conditions for quest
	//conditioni::add("IfEatSupply", if_eat_supply, Supply);
	//conditioni::add("IfEntry", if_tag, AnswerEntry);
	//conditioni::add("IfStory", if_story);
	//conditioni::add("IfTreasure", if_treasure);
	//conditioni::add("IfVisit", if_visit);
}

BSDATA(scripti) = {
	{"Nickname", set_value}, // Value - 0
	{"NicknameEnd", set_value}, // Value - 1
	{"AddGun", add_gun, 1, choose_add_gun},
	{"AddGunUnloaded", add_gun, 0, choose_add_gun},
	{"AddTile", add_tile},
	{"Block", block_action},
	{"BonusToAll", add_round_bonus, -1, 0, FG(TipsInfo)},
	{"BonusToExploration", add_round_bonus, Exploration, 0, FG(TipsInfo)},
	{"Bury", bury, 0, 0, FG(TipsInfo)},
	{"CheckDanger", check_danger},
	{"Choose", choose_case},
	{"ChooseCounter", choose_counter, 1},
	{"ChooseCustom", choose_custom, AnswerCustom},
	{"CounterA", set_counter, 0},
	{"CounterB", set_counter, 1},
	{"CounterC", set_counter, 2},
	{"CounterD", set_counter, 3},
	{"CounterE", set_counter, 4},
	{"CounterName", counter_name},
	{"CounterX", set_counter, -1},
	{"Damage", damage},
	{"EatSupply", eat_supply, Supply, 0, FG(TipsInfo), eat_supply_info},
	{"Entry", entry},
	{"FullThrottle", full_throttle},
	{"Goal", set_goal},
	{"IfChoosedAction", if_choosed_action},
	{"IfEqual", if_equal, 0},
	{"IfLast", if_last},
	{"IfNonZeroForward", if_non_zero_forward},
	{"IfNotSail", if_not_sail},
	{"IfZeroForward", if_zero_forward},
	{"LostGame", end_game, 0},
	{"MarkEntry", mark_entry, AnswerEntry},
	{"MarkVisit", mark_visit},
	{"MoveToPlayer", tile_move_to_player},
	{"Name", set_value, AnswerName},
	{"Page000", run_script, 0},
	{"Page100", run_script, 100},
	{"Page200", run_script, 200},
	{"Page300", run_script, 300},
	{"Page400", run_script, 400},
	{"Page500", run_script, 500},
	{"Page600", run_script, 600},
	{"Page700", run_script, 700},
	{"Page800", run_script, 800},
	{"Page900", run_script, 900},
	{"PageForward", page_forward, 0},
	{"PageNext", page_forward, 1},
	{"PenaltyA", minus_counter, 0},
	{"PenaltyB", minus_counter, 1},
	{"PenaltyC", minus_counter, 2},
	{"PenaltyD", minus_counter, 3},
	{"PenaltyE", minus_counter, 4},
	{"PlayStars", play_stars},
	{"ReloadGun", reload_gun_or_add, 0, choose_reload_gun, FG(TipsInfo)},
	{"ReloadGunOrHull", reload_gun_or_add, Hull, 0, FG(TipsInfo)},
	{"RemoveAllNavigation", remove_all_navigation},
	{"RemoveGun", remove_gun},
	{"RemoveTile", remove_tile},
	{"Roll", make_roll},
	{"RollGuns", make_roll, 1},
	{"RollSilent", make_roll_silent},
	{"Sail", sail_away},
	{"Scene", change_scene},
	{"Scout", scout_area},
	{"SetShip", set_ship},
	{"SetVisit", set_visit},
	{"ShowMap", show_map},
	{"Skill", add_skill},
	{"StopActions", stop_actions},
	{"Tile000", apply_tile, 0},
	{"Tile900", apply_tile, 900},
	{"TileRock", apply_tile, 65535},
	{"TradeFriend", trade_friend},
	{"Treasure", add_treasure, 0, 0, FG(TipsInfo) | FG(TipsInfoBonus)},
	{"UpgradeGun", upgrade_gun},
	{"WinGame", end_game, 1},
	{"ZeroCounters", zero_counters},
	{"ZeroDanger", set_ability, Danger},
	{"ZeroRerollIfNot", set_ability_if_not, Reroll},
	{"ZeroSupplyIfNot", set_ability_if_not, Supply, 0, FG(TipsInfo), set_ability_if_not_info},
};
BSDATAF(scripti)