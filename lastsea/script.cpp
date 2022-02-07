#include "condition.h"
#include "counters.h"
#include "main.h"
#include "pathfind.h"
#include "script.h"

gamei game;
static int last_counter, last_value, last_action;
static char round_skills_bonus[Navigation + 1];
static bool	need_sail, need_stop, need_stop_actions;
static tilei* last_tile;
static int game_result;
static ability_s last_ability;
static int prop_end_scene, prop_visit, prop_maximum_danger;
static const quest* last_location;
static const quest* new_location;
counters variables;

void print(stringbuilder& sb, const variants& source);
void main_menu();

static void clear_message() {
	utg::sb.clear();
}

static void stop_and_clear(const char* format) {
	if(!utg::sb)
		return;
	if(!format)
		format = getnm("Continue");
	draw::pause(format);
	clear_message();
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
	if(p->is(VisitManyTimes))
		sb.adds(getnm(bsdata<speciali>::elements[VisitManyTimes].id));
	an.add(p, temp);
}

static void apply_effect(const variants& tags) {
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
	auto push_last = quest::last;
	quest::last = p;
	for(auto v : tags) {
		conditioni* pc = v;
		if(!pc)
			break;
		if(!pc->proc(v.counter, pc->param))
			return false;
	}
	quest::last = push_last;
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
	if(p && p->next > 0)
		return find_promt(p->next);
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

static void add_message(const char* format) {
	if(!format)
		return;
	clear_message();
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
		draw::pause();
		clear_message();
		format = pn;
	}
	game.act(format);
}

static void apply_effect(const quest* p) {
	while(p) {
		quest::last = p;
		add_header(p);
		if(p->text && p->next != AnswerChoose)
			add_message(p->text);
		apply_effect(p->tags);
		p = apply_answers(p);
	}
}

static bool allow_choose(ability_s v, int bonus) {
	switch(v) {
	case Infamy: case Discontent: case Danger:
		return true;
	default:
		return (game.get(v) + bonus) >= 0;
	}
}

static bool allow_choose(special_s v, int bonus) {
	switch(v) {
	case EatSupply:
		return allow_choose(Supply, -game.getmaximum(Eat));
	case ReloadGun:
		if(bonus >= 0)
			return game.reloadgun(4, false);
		else
			return game.unloadgun(4, false);
		break;
	case AddGun:
		return game.addgun(bonus, true, false);
	default:
		return true;
	}
}

static bool allow_choose(variant v) {
	switch(v.type) {
	case Ability: return allow_choose((ability_s)v.value, v.counter);
	case Special: return allow_choose((special_s)v.value, v.counter);
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
			if(p->is(VisitManyTimes))
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
		auto push_last = quest::last;
		apply_choose(p, p->text, count);
		quest::last = push_last;
	}
}

static bool apply_choose(int page, int count, const char* cancel) {
	auto r = false;
	auto p = find_promt(page);
	if(p) {
		auto push_last = quest::last;
		r = apply_choose(p, p->text, count, cancel);
		quest::last = push_last;
	}
	return r;
}

static const quest* find_forward(int bonus) {
	auto pe = bsdata<quest>::end();
	auto index = AnswerForward + bonus;
	for(auto p = quest::last + 1; p < pe; p++) {
		if(p->next != -1)
			continue;
		if(p->index < AnswerForward || p->index >= AnswerForward + 100)
			break;
		if(p->index == index)
			return p;
	}
	return 0;
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

static bool allow_action(ability_s v, int bonus) {
	switch(v) {
	case Mission: case Cabine:
		return (game.get(v) + bonus) <= game.getmaximum(v);
	default: return true;
	}
}

static bool allow_action(variant v) {
	switch(v.type) {
	case Ability: return allow_action((ability_s)v.value, v.counter);
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
			if(p->is(VisitManyTimes))
				return true;
			if(ismarked(index))
				return false;
			return true;
		}
	};
	if(!last_location)
		return;
	clear_message();
	quest::last = last_location;
	add_header(quest::last);
	if(quest::last->text)
		game.act(quest::last->text);
	apply_effect(quest::last->tags);
	game.clearactions();
	handler san;
	auto index = quest::last->index;
	auto pe = bsdata<quest>::end();
	for(auto p = quest::last + 1; p < pe; p++) {
		if(p->index != index)
			break;
		if(p->is(VisitRequired) && count > 0) {
			if(!p->is(NotUseAction))
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

static void add_treasure(answers& an, trigger_s trigger) {
	for(auto& e : bsdata<treasurei>()) {
		if(!e.isactive() || e.isdiscarded())
			continue;
		if(e.trigger != trigger)
			continue;
		add_treasure(an, &e);
	}
}

static void add_treasure(answers& an, trigger_s trigger, ability_s ability, useda& used) {
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
		clear_message();
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
		clear_message();
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

static void sail_ship(int bonus) {
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
	}
}

static void special_mission(int v, int* pages) {
	v = v - 1;
	if(v >= 0 && v < 5)
		game.script(pages[v]);
}

static void captain_cabine() {
	static int pages[] = {43, 44, 45, 46, 47};
	special_mission(game.get(Cabine), pages);
}

static void captain_mission() {
	static int pages[] = {48, 49, 50, 51, 52};
	special_mission(game.get(Mission), pages);
}

static void global_threat() {
	static int pages[] = {791, 792, 793, 794, 795};
	stop_and_clear(getnm("GloablThreat"));
	special_mission(game.get(Threat), pages);
}

static void choose_damage(const char* title, int count, const slice<ability_s>& source) {
	answers an;
	char temp[260]; stringbuilder sb(temp);
	while(count > 0 && !draw::isnext()) {
		an.clear();
		for(auto v : source) {
			if(game.get(v) > 0) {
				auto p = bsdata<abilityi>::elements + v;
				an.add(p, "%1%+2i", getnm(p->id), -1);
			}
		}
		sb.clear();
		sb.add(title, count);
		auto p = (abilityi*)an.choose(temp);
		auto v = (ability_s)(p - bsdata<abilityi>::elements);
		game.set(v, game.get(v) - 1);
		count--;
	}
}

int gamei::getpage() {
	if(!last_location)
		return 0;
	return last_location->index;
}

void treasurei::gaining() const {
	apply_effect(gain);
}

void treasurei::lossing() const {
	apply_effect(loss);
}

void treasurei::apply() const {
	apply_effect(use);
}

void pirate::afterchange(ability_s v, int b) {
	switch(v) {
	case Exploration: case Brawl: case Hunting:
	case Aim: case Swagger: case Navigation:
		checkexperience(v);
		break;
	case Mission:
		captain_mission();
	case Cabine:
		captain_cabine();
		break;
	case Threat:
		global_threat();
		break;
	}
	if(b > 0)
		apply(WhenAbilityIncreased, v);
	else if(b < 0)
		apply(WhenAbilityDecreased, v);
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
}

void pirate::roll(int mode) {
	makeroll(mode);
	confirmroll();
	apply_roll_result(quest::last, last_result);
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

void gamei::sfgetproperty(const void* object, variant v, stringbuilder& sb) {
	switch(v.type) {
	case Ability:
		if(v.value >= Gun1 && v.value <= Gun4) {
			auto level = game.getgunlevel(v.value - Gun1);
			auto loaded = game.isgunloaded(v.value - Gun1);
			if(level > 0)
				sb.add(getnm("GunStatus"), level, getnm(loaded ? "Loaded" : "Unloaded"));
		} else
			pirate::sfgetproperty(static_cast<pirate*>(&game), v, sb);
		break;
	}
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

void gamei::choosecounter() {
	answers an;
	for(auto i = 0; i < variables.getcount(); i++) {
		auto pn = variables.getname(i);
		if(!pn)
			continue;
		an.add((void*)i, pn);
	}
	last_counter = (int)an.choose(getnm("ChooseTarget"));
}

bool gamei::ischoosed(int i) const {
	for(auto v : actions) {
		if(v == i)
			return true;
	}
	return false;
}

static void test_last_action() {
	const auto m = sizeof(game.actions) / sizeof(game.actions[0]);
	if(last_action >= m - 1)
		return;
	if(game.actions[last_action] == game.actions[last_action + 1])
		need_stop = true;
}

static void show_map() {
	oceani::createobjects();
	oceani::showsplash();
}

static void special_command(special_s v, int bonus) {
	switch(v) {
	case PageForward:
		apply_effect(find_forward(bonus));
		break;
	case PageNext:
		draw::pause();
		apply_effect(find_forward(bonus));
		break;
	case AddGun:
		if(game.addgun(bonus, true, true))
			game.information(getnm("GunAdded"), bonus);
		break;
	case AddGunUnloaded:
		if(game.addgun(bonus, false, true))
			game.information(getnm("GunAddedUnloaded"), bonus);
		break;
	case ReloadGun:
	case ReloadGunOrHull:
		if(bonus >= 0) {
			if(game.reloadgun(4, true))
				game.information(getnm("GunReloaded"));
			else if(v == ReloadGunOrHull)
				game.set(Hull, game.get(Hull) + 1);
		} else {
			if(game.unloadgun(4, true))
				game.warning(getnm("GunUnloaded"));
			else if(v == ReloadGunOrHull)
				game.set(Hull, game.get(Hull) - 1);
		}
		break;
	case EatSupply:
		bonus += game.getmaximum(Eat);
		if(game.get(Supply) >= bonus)
			game.set(Supply, game.get(Supply) - bonus);
		else
			need_stop = true;
		break;
	case FullThrottle:
		sail_ship(bonus);
		break;
	case TradeFriend:
		game.tradefriend();
		break;
	case Sail:
		if(bonus > 0)
			clear_message();
		last_tile = sail_next_hexagon();
		stop_and_clear(getnm("SailAway"));
		break;
	case ShowMap:
		show_map();
		break;
	case ZeroCounters:
		variables.clear();
		break;
	case Name:
		last_value = AnswerName + bonus;
		break;
	case Entry:
		last_value = game.istag(AnswerEntry + bonus) ? 1 : 0;
		break;
	case CounterName:
		variables.setname(bonus, quest::getname(last_value));
		break;
	case ChooseCounter:
		game.choosecounter();
		break;
	case VisitManyTimes: case VisitRequired:
		break;
	case StopActions:
		need_stop_actions = true;
		break;
	case ChooseCustom:
		apply_choose(AnswerCustom + bonus, 1);
		break;
	case MarkVisit:
		if(!bonus)
			bonus = 1;
		addnumber(quest::last->index, prop_visit, bonus);
		break;
	case SetVisit:
		if(!bonus)
			removenumber(quest::last->index, prop_visit);
		else
			setproperty(quest::last->index, prop_visit, bonus);
		break;
	case IfLast:
		test_last_action();
		break;
	case RemoveAllNavigation:
		remove_active_tiles(1, 30);
		break;
	default:
		game.warning(getnm("UnknownCommand"), v, bonus);
		break;
	}
}

static void ability_command(ability_s v, int bonus) {
	last_ability = v;
	if(bonus) {
		game.set(last_ability, game.get(last_ability) + bonus);
		last_value = game.get(last_ability);
	}
}

static void script_command(int type, int bonus) {
	auto& ei = bsdata<scripti>::elements[type];
	ei.proc(bonus, ei.param);
}

void gamei::apply(variant v) {
	switch(v.type) {
	case Ability: ability_command((ability_s)v.value, v.counter); break;
	case Special: special_command((special_s)v.value, v.counter); break;
	case Card: game.gaintreasure((treasurei*)v.getpointer()); break;
	case Goal: game.setgoal((goali*)v.getpointer()); break;
	case Script: script_command(v.value, v.counter); break;
	}
}

static bool treasure_active(const treasurei* p) {
	return p && p->isactive() && !p->isdiscarded();
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
	return quest::last && (getnumber(quest::last->index, prop_visit) == counter);
}

static void bury(int bonus, int param) {
	game.bury(bonus);
}

static void end_game(int bonus, int param) {
	game_result = param;
	draw::pause(getnm(param > 0 ? "WinGame" : "LostGame"));
	draw::setnext(main_menu);
}

static void run_script(int bonus, int param) {
	game.script(param + bonus);
}

static void set_value(int bonus, int param) {
	if(bonus >= 0)
		last_value = param + bonus;
}

static void set_ability(int bonus, int param) {
	last_value = bonus;
	game.set((ability_s)param, bonus);
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
	apply_choose(quest::last, 0, bonus);
}

static void make_roll(int bonus, int param) {
	game.roll(bonus);
}

static void make_roll_silent(int bonus, int param) {
	rollv(bonus);
	apply_roll_result(quest::last, last_result);
}

static void damage(int bonus, int param) {
	if(!bonus)
		bonus = last_value;
	if(bonus)
		apply_choose(6200, bonus);
	else
		draw::pause();
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

static void set_ability_if_not(int bonus, int param) {
	if(game.get((ability_s)param) > bonus) {
		game.set((ability_s)param, bonus);
		need_stop = true;
	}
}

static void check_danger(int bonus, int param) {
	auto m = game.getmaximum(Danger);
	auto v = game.get(Danger);
	if(v >= m) {
		game.set(Danger, 0);
		game.set(Threat, game.get(Threat) + 1);
	}
}

static void correct(int value, int& bonus, int min, int max) {
	if(value + bonus > max) {
		bonus = max + bonus - value;
		if(bonus < 0)
			bonus = 0;
	} else if(value + bonus < min) {
		bonus = min + bonus - value;
		if(bonus > 0)
			bonus = 0;
	}
}

static void minus_counter(int bonus, int param) {
	last_value -= variables.get(param);
}

static void set_counter(int bonus, int param) {
	if(param != -1)
		last_counter = param;
	last_value = variables.get(last_counter);
	correct(last_value, bonus, 0, 14);
	if(bonus) {
		last_value += bonus;
		auto pn = variables.getname(last_counter);
		if(pn)
			game.information("%1%+2i", pn, bonus);
		variables.set(last_counter, bonus);
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

static void set_ship(int bonus, int param) {
	game.setmarker(bonus);
}

static void add_tile(int bonus, int param) {
	last_tile = tilei::pick(last_value);
	if(last_tile)
		last_tile->index = bonus;
}

static void tile_move_to_player(int bonus, int param) {
	last_tile = tilei::find(last_value);
	if(!last_tile)
		return;
	stop_and_clear(0);
	last_tile->moveto(game.getmarker(), bonus);
	if(last_tile->index == game.getmarker())
		game.script(last_tile->param);
}

void initialize_script() {
	// Properties
	prop_end_scene = propertyi::add("EndScene", propertyi::Number);
	prop_visit = propertyi::add("Visit", propertyi::Number);
	prop_maximum_danger = propertyi::add("MaximumDanger", propertyi::Number);
	// Prompt conditions for quest
	conditioni::add("IfEntry", if_tag, AnswerEntry);
	conditioni::add("IfStory", if_story);
	conditioni::add("IfTreasure", if_treasure);
	conditioni::add("IfVisit", if_visit);
}

BSDATA(scripti) = {
	{"AddTile", add_tile},
	{"Block", block_action},
	{"BonusToAll", add_round_bonus, -1},
	{"BonusToExploration", add_round_bonus, Exploration},
	{"Bury", bury},
	{"CheckDanger", check_danger},
	{"Choose", choose_case},
	{"CounterA", set_counter, 0},
	{"CounterB", set_counter, 1},
	{"CounterC", set_counter, 2},
	{"CounterD", set_counter, 3},
	{"CounterE", set_counter, 4},
	{"CounterX", set_counter, -1},
	{"Damage", damage},
	{"IfChoosedAction", if_choosed_action},
	{"IfEqual", if_equal, 0},
	{"IfNonZeroForward", if_non_zero_forward},
	{"IfNotSail", if_not_sail},
	{"IfZeroForward", if_zero_forward},
	{"LostGame", end_game, 0},
	{"MarkEntry", mark_entry, AnswerEntry},
	{"MoveToPlayer", tile_move_to_player},
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
	{"PenaltyA", minus_counter, 0},
	{"PenaltyB", minus_counter, 1},
	{"PenaltyC", minus_counter, 2},
	{"PenaltyD", minus_counter, 3},
	{"PenaltyE", minus_counter, 4},
	{"Roll", make_roll},
	{"RollGuns", make_roll, 1},
	{"RollSilent", make_roll_silent},
	{"Scene", change_scene},
	{"Scout", scout_area},
	{"SetShip", set_ship},
	{"Skill", add_skill},
	{"Tile000", apply_tile, 0},
	{"Tile900", apply_tile, 900},
	{"TileRock", apply_tile, 65535},
	{"WinGame", end_game, 1},
	{"ZeroDanger", set_ability, Danger},
	{"ZeroRerollIfNot", set_ability_if_not, Reroll},
	{"ZeroSupplyIfNot", set_ability_if_not, Supply},
};
BSDATAF(scripti)