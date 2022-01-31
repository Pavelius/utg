#include "counters.h"
#include "main.h"
#include "pathfind.h"

gamei		game;
int			last_choose;
static int	last_tile, last_counter, last_name;
static bool	need_sail, need_stop, need_stop_actions;
ability_s	last_ability;
const quest* last_quest;
const quest* last_location;
const quest* next_quest;
static counters variables;

static void clear_message() {
	utg::sb.clear();
}

static void add_header(const quest* ph) {
	if(ph->header)
		utg::header = ph->header;
	if(ph->image)
		utg::url = ph->image;
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
	last_ability = Infamy;
	need_stop = false;
	for(auto v : tags) {
		if(next_quest)
			break;
		if(need_stop)
			break;
		game.apply(v);
	}
	need_stop = push_stop;
}

static void apply_answers(const quest* ph) {
	if(!ph)
		return;
	answers an;
	auto index = ph->index;
	auto pe = bsdata<quest>::end();
	for(auto p = ph + 1; p < pe; p++) {
		if(p->index != index)
			break;
		if(p->next < 30)
			continue;
		add_answer(an, p);
	}
	if(an) {
		auto p = (quest*)utg::choose(an, 0);
		if(p && p->next > 0) {
			next_quest = quest::find(p->next);
			if(next_quest && next_quest->text)
				clear_message();
		}
	}
}

static void apply_choose(const quest* ph, int count);

static void apply_effect() {
	while(next_quest) {
		last_quest = next_quest; next_quest = 0;
		add_header(last_quest);
		if(last_quest->text && last_quest->next) {
			clear_message();
			game.actn(utg::sb, last_quest->text, 0);
		}
		apply_effect(last_quest->tags);
		if(last_choose) {
			auto n = last_choose; last_choose = 0;
			apply_choose(last_quest, n);
		}
		if(!next_quest)
			apply_answers(last_quest);
	}
}

static void apply_effect(const quest* p) {
	next_quest = p;
	apply_effect();
}

static bool allow_choose(ability_s v, int bonus) {
	switch(v) {
	case Infamy: return true;
	default: return (game.get(v) + bonus) >= 0;
	}
}

static bool allow_choose(special_s v, int bonus) {
	switch(v) {
	case ReloadGun:
		if(bonus >= 0)
			return game.reloadgun(4, false);
		else
			return game.unloadgun(4, false);
		break;
	case AddGun: return game.addgun(bonus, true, false);
	default: return true;
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

static void apply_choose(const quest* ph, int count) {
	struct handler : utg::choosei {
		void apply(int index, const void* object) override {
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
		handler(answers& an) : choosei(an) {}
	};
	if(!ph || !count)
		return;
	answers an; handler san(an);
	auto index = ph->index;
	auto pe = bsdata<quest>::end();
	for(auto p = ph + 1; p < pe; p++) {
		if(p->index != index)
			break;
		if(p->next)
			continue;
		add_answer(an, p);
	}
	san.choose(0, count);
}

static const quest* find_condition(int bonus) {
	auto pe = bsdata<quest>::end();
	auto index = 1000 + bonus;
	for(auto p = last_quest + 1; p < pe; p++) {
		if(p->next != -1)
			continue;
		if(p->index < 1000 || p->index >= 1100)
			break;
		if(p->index == index)
			return p;
	}
	return 0;
}

static void apply_condition(int bonus) {
	auto p = find_condition(bonus);
	if(p)
		next_quest = p;
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
	for(auto p = last_quest + 1; p < pe; p++) {
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
	struct handler : utg::choosei {
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
		handler(answers& an) : choosei(an) {}
	};
	if(!last_location)
		return;
	clear_message();
	last_quest = last_location; next_quest = 0;
	add_header(last_quest);
	if(last_quest->text)
		game.actn(utg::sb, last_quest->text, 0);
	apply_effect(last_quest->tags);
	answers an;
	game.clearactions();
	handler san(an);
	auto index = last_quest->index;
	auto pe = bsdata<quest>::end();
	for(auto p = last_quest + 1; p < pe; p++) {
		if(p->index != index)
			break;
		if(p->is(VisitRequired) && count > 0) {
			count--;
			san.apply(0, p);
			continue;
		}
		add_answer(an, p);
	}
	san.choose(getnm("WhatDoYouWantToVisit"), count);
	game.sortactions();
}

static void play_actions() {
	need_stop_actions = false;
	for(auto v : game.actions) {
		if(!v)
			continue;
		auto p = bsdata<quest>::elements + v;
		if(!p->next)
			continue;
		clear_message();
		apply_effect(quest::find(p->next));
		if(need_stop_actions)
			break;
		utg::pause(getnm("NextAction"));
	}
}

static void apply_sail();
static void apply_scene();

static void end_scene() {
	if(!last_location || !last_location->next)
		return;
	apply_effect(quest::find(last_location->next));
	if(need_sail) {
		need_sail = false;
		utg::pause(getnm("SailAway"));
		draw::setnext(apply_sail);
	} else
		draw::setnext(apply_scene);
}

static void apply_scene() {
	choose_actions(player_count);
	play_actions();
	end_scene();
}

static void apply_sail() {
	auto index = game.oceani::chooseroute(0, 1);
	if(index == pathfind::Blocked)
		return;
	game.setmarker(index);
	auto tile_id = game.getlocation(index);
	if(!tile_id) {
		game.setlocation(index, game.picktile());
		game.createobjects();
		game.showsplash();
	}
	tile_id = game.getlocation(index);
	if(!tile_id)
		return;
	clear_message();
	game.script(tile_id);
}

static void change_scene(int value) {
	auto p = quest::find(4000 + value);
	if(value && last_location != p) {
		last_location = p;
		game.unlockall();
	}
	utg::pause(getnm("NextScene"));
	draw::setnext(apply_scene);
}

static void special_mission(int v, int* pages) {
	v = v - 1;
	if(v >= 0 && v < 5)
		next_quest = quest::find(pages[v]);
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
	special_mission(game.get(Threat), pages);
}

static void choose_damage(int count, const slice<ability_s>& source) {
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
		sb.add(getnm("DistributeDamage"), count);
		auto p = (abilityi*)utg::choose(an, temp);
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

void pirate::afterchange(ability_s v) {
	switch(v) {
	case Exploration: case Brawl: case Hunting: case Aim: case Swagger: case Navigation:
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
}

static void generate_classes() {
	auto push_interactive = utg::interactive;
	utg::interactive = false;
	for(auto& e : bsdata<pirate>())
		e.generate();
	utg::interactive = push_interactive;
}

void pirate::roll(special_s type) {
	makeroll(type);
	confirmroll();
	last_bonus = 0;
	apply_roll_result(last_quest, last_result);
}

void gamei::script(int page) {
	auto p = quest::find(page);
	if(p)
		apply_effect(p);
}

void gamei::clear() {
	memset(this, 0, sizeof(*this));
	shiplog::clear();
}

void gamei::generate() {
	for(auto& e : game.pirates) {
		e.clear();
		e.randomname();
	}
	game.pirate::clear();
	game.pirate::randomname();
	game.pirate::generate();
}

void gamei::choosehistory() {
	game.pirate::choosehistory();
	clear_message();
}

void gamei::createtreasure() {
	auto m = bsdata<treasurei>::source.getcount();
	for(auto& e : bsdata<treasurei>()) {
		if(e.ismagic())
			continue;
		if(e.isstory())
			continue;
		treasures.add(bsdata<treasurei>::source.indexof(&e));
	}
	zshuffle(treasures.data, treasures.count);
}

void gamei::createtiles() {
	tiles.clear();
	for(auto i = 1; i <= 30; i++)
		tiles.add(i);
	zshuffle(tiles.data, tiles.count);
}

indext gamei::picktile() {
	if(!tiles)
		return 0;
	auto i = tiles.data[0];
	tiles.remove(0);
	return i;
}

const treasurei* gamei::picktreasure() {
	if(!treasures.count)
		return 0;
	auto pv = bsdata<treasurei>::elements + treasures.data[0];
	treasures.remove(0);
	return pv;
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

void gamei::fullthrottle(int level) {
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
		auto tile = picktile();
		if(!tile)
			break;
		setlocation(index, tile);
		count--;
	}
	createobjects();
	showsplash();
}

void gamei::showseamap() {
	game.oceani::showseamap();
}

void gamei::listofcounters() {
	char temp[64]; stringbuilder sb(temp);
	for(unsigned i = 0; i < 16; i++) {
		auto pn = variables.getname(i);
		if(!pn)
			continue;
		auto v = variables.get(i);
		sb.clear(); sb.add("%1i", v);
		draw::label(pn, temp, 0);
	}
}

void gamei::choosecounter() {
	answers an;
	for(auto i = 0; i < 16; i++) {
		auto pn = variables.getname(i);
		if(!pn)
			continue;
		an.add((void*)i, pn);
	}
	last_counter = (int)utg::choose(an, getnm("ChooseTarget"));
}

bool gamei::ischoosed(int i) const {
	for(auto v : actions) {
		if(v == i)
			return true;
	}
	return false;
}

static ability_s standart_damage[] = {Hull, Crew, Supply};

static void special_command(special_s v, int bonus) {
	switch(v) {
	case Roll:
	case RollGuns:
		game.roll(v);
		break;
	case RollSilent:
		rollv(0);
		apply_roll_result(last_quest, last_result);
		break;
	case Choose:
		last_choose = bonus;
		break;
	case Skill:
		game.raiseskills(bonus);
		break;
	case Scout:
		game.chartacourse(bonus);
		break;
	case SetShip:
		game.setmarker(bonus);
		break;
	case Tile000: last_tile = bonus; break;
	case Tile900: last_tile = 900 + bonus; break;
	case TileRock: last_tile = pathfind::Blocked; break;
	case AddTile:
		game.setlocation(bonus, last_tile);
		break;
	case Page000: case Page100: case Page200: case Page300: case Page400:
	case Page500: case Page600: case Page700: case Page800: case Page900:
		next_quest = quest::find((v - Page000) * 100 + bonus);
		if(next_quest && next_quest->text)
			clear_message();
		break;
	case Scene:
		change_scene(bonus);
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
	case Bury:
		game.bury(bonus);
		break;
	case Block:
		if(bonus > 0)
			game.lock(bonus - 1);
		else if(bonus < 0)
			game.unlock(bonus - 1);
		break;
	case PaySupply:
		if(game.get(Supply) >= bonus)
			game.set(Supply, game.get(Supply) + bonus);
		break;
	case PaySupplyEat:
		bonus += game.getmaximum(Eat);
		if(game.get(Supply) >= bonus)
			game.set(Supply, game.get(Supply) + bonus);
		break;
	case ZeroSupplyOrDiscontent:
		if(game.get(Supply) == 0)
			game.set(Discontent, game.get(Discontent) + bonus);
		else
			game.set(Supply, 0);
		break;
	case FullThrottle:
		game.fullthrottle(bonus);
		break;
	case TradeFriend:
		game.tradefriend();
		break;
	case Sail:
		need_sail = true;
		break;
	case ZeroCounters:
		variables.clear();
		break;
	case PenaltyA: case PenaltyB: case PenaltyC: case PenaltyD:
		last_bonus -= variables.get(v - PenaltyA);
		break;
	case CounterA: case CounterB: case CounterC: case CounterD: case CounterX:
		if(v != CounterX)
			last_counter = v - CounterA;
		if(bonus > 0 || bonus < 0) {
			auto pn = variables.getname(last_counter);
			if(pn)
				game.information("%1%+2i", pn, bonus);
			variables.add(last_counter, bonus);
		}
		break;
	case Name:
		last_name = 6000 + bonus;
		break;
	case CounterName:
		variables.setname(bonus, quest::getname(last_name));
		break;
	case ChooseCounter:
		game.choosecounter();
		break;
	case VisitManyTimes: case VisitRequired:
		break;
	case IfSail:
		if(need_sail)
			apply_condition(bonus);
		break;
	case IfExistEntry:
		if(!last_name)
			game.warning(getnm("NotDefinedName"));
		else {
			if(game.istag(last_name))
				apply_condition(bonus);
		}
		break;
	case IfCounterZero:
		if(variables.get(last_counter) <= 0)
			apply_condition(bonus);
		break;
	case IfChoosedAction:
		if(bonus > 0) {
			if(!game.ischoosed(bsdata<quest>::source.indexof(find_action(bonus + 1))))
				need_stop = true;
		}
		else if(bonus < 0) {
			if(game.ischoosed(bsdata<quest>::source.indexof(find_action(bonus + 1))))
				need_stop = true;
		}
		break;
	case StopActions:
		need_stop_actions = true;
		break;
	case Damage:
		choose_damage(bonus, standart_damage);
		break;
	case CheckDanger:
		// TODO
		break;
	case PlayStars:
		// TODO
		break;
	default:
		game.warning(getnm("UnknownCommand"), v, bonus);
		break;
	}
}

void gamei::apply(variant v) {
	switch(v.type) {
	case Ability:
		last_ability = (ability_s)v.value;
		if(v.counter)
			game.set(last_ability, game.get(last_ability) + v.counter);
		break;
	case Special: special_command((special_s)v.value, v.counter); break;
	case Card: game.gaintreasure((treasurei*)v.getpointer()); break;
	case Goal: game.setgoal((goali*)v.getpointer()); break;
	}
}