#include "category.h"
#include "counters.h"
#include "main.h"
#include "pathfind.h"

gamei game;
static int last_tile, last_counter, last_value, last_action;
static int round_skill_bonus;
static bool	need_sail, need_stop, need_stop_actions;
static ability_s last_ability;
static npcname* active_npc;
const quest* last_quest;
const quest* last_location;
static const quest* new_location;
counters variables;

void print(stringbuilder& sb, const variants& source);

void gamei::act(const char* format) {
	if(active_npc)
		active_npc->actn(utg::sb, format, 0);
	else
		game.npcname::actn(utg::sb, format, 0);
}

static void clear_message() {
	utg::sb.clear();
}

static void add_header(const quest* ph) {
	if(ph->header)
		answers::header = ph->header;
	if(ph->image)
		answers::resid = ph->image;
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

static bool allow_promt(const quest& e) {
	for(auto v : e.tags) {
		if(v.type != Special)
			continue;
		switch(v.value) {
		case IfVisit: return category::get(e.index) == v.counter;
		default: break;
		}
	}
	return true;
}

const quest* find_promt(int index) {
	for(auto& e : bsdata<quest>()) {
		if(e.next != -1)
			continue;
		if(e.index != index)
			continue;
		if(!allow_promt(e))
			continue;
		return &e;
	}
	return 0;
}

static const quest* find_scene(int index) {
	for(auto& e : bsdata<quest>()) {
		if(e.index != index)
			continue;
		if(!allow_promt(e))
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

static void apply_choose(const quest* ph, const char* title, int count);

static void apply_effect(const quest* p) {
	while(p) {
		last_quest = p;
		add_header(p);
		if(p->text && p->next != AnswerChoose) {
			clear_message();
			game.act(p->text);
		}
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

static void apply_choose(const quest* ph, const char* title, int count) {
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
		return;
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
	if(index >= 5000) {
		auto need_bonus = index - 5000;
		for(auto p : game.gettreasures()) {
			if(p->trigger != WhenChooseSpecial)
				continue;
			if(p->bonus != need_bonus)
				continue;
			add_treasure(san, p, p->use);
		}
	}
	san.choose(title, count);
}

static void apply_choose(int page, int count) {
	auto p = find_promt(page);
	if(p) {
		auto push_last = last_quest;
		apply_choose(p, p->text, count);
		last_quest = push_last;
	}
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

static void apply_forward(int bonus) {
	auto p = find_forward(bonus);
	if(p) {
		apply_effect(p);
		need_stop = true;
	}
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
	last_quest = last_location;
	add_header(last_quest);
	if(last_quest->text)
		game.act(last_quest->text);
	apply_effect(last_quest->tags);
	game.clearactions();
	handler san;
	auto index = last_quest->index;
	auto pe = bsdata<quest>::end();
	for(auto p = last_quest + 1; p < pe; p++) {
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
	for(auto p : game.gettreasures()) {
		if(p->trigger != trigger)
			continue;
		add_treasure(an, p);
	}
}

static void add_treasure(answers& an, trigger_s trigger, ability_s ability, useda& used) {
	for(auto p : game.gettreasures()) {
		if(p->trigger != trigger)
			continue;
		if(p->ability != ability)
			continue;
		if(used.is(p))
			continue;
		an.add(p, getnm("UseTreasureToGainBonus"), getnm(p->id), p->bonus);
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

static void change_scene(int value) {
	auto p = find_scene(4000 + value);
	if(value && last_location != p) {
		new_location = p;
		game.unlockall();
		variables.clear();
	}
	summary_action("NextScene", WhenUse);
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
		active_npc = &game.getactor(parcipant_index);
		clear_message();
		game.script(p->next);
		if(need_stop_actions)
			break;
		summary_action("NextAction", WhenUse);
		parcipant_index++;
	}
	active_npc = 0;
}

void start_scene() {
	if(need_sail) {
		need_sail = false;
		clear_message();
		game.script(last_tile);
		draw::setnext(start_scene);
	} else if(new_location) {
		last_location = new_location;
		new_location = 0;
		round_skill_bonus = 0;
		choose_actions(player_count);
		play_actions();
		if(last_location && last_location->next)
			game.script(last_location->next);
		draw::setnext(start_scene);
	}
}

static int sail_next_hexagon() {
	auto index = game.oceani::chooseroute(0, 1);
	if(index == pathfind::Blocked)
		return 0;
	game.setmarker(index);
	auto tile_id = game.getlocation(index);
	if(!tile_id) {
		game.setlocation(index, game.picktile());
		game.createobjects();
		game.showsplash();
	}
	need_sail = true;
	return game.getlocation(index);
}

static void sail_ship(int bonus) {
	while(!draw::isnext()) {
		if(!draw::yesno(getnm("DoYouWantToMoveShip")))
			return;
		if(bonus == 0) {
			apply_choose(AnswerCustom + 3, 3);
			last_tile = sail_next_hexagon();
			break;
		} else if(bonus == 1) {
			apply_choose(AnswerCustom + 3, 2);
			last_tile = sail_next_hexagon();
		} else {
			apply_choose(AnswerCustom + 4, 1);
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

void pirate::makeroll(special_s type) {
	char temp[260]; stringbuilder sb(temp);
	last_bonus = 0;
	if(last_ability >= Exploration && last_ability <= Navigation) {
		last_bonus += round_skill_bonus;
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
		if(type == RollGuns && !gun_used) {
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

void pirate::roll(special_s type) {
	makeroll(type);
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

static void check_danger() {
	auto m = game.getmaximum(Danger);
	auto v = game.get(Danger);
	if(v >= m) {
		game.set(Danger, 0);
		game.set(Threat, game.get(Threat) + 1);
	}
}

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
		apply_choose(last_quest, 0, bonus);
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
		game.script((v - Page000) * 100 + bonus);
		break;
	case PageForward:
		apply_effect(find_forward(bonus));
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
	case EatSupply:
		bonus += game.getmaximum(Eat);
		if(game.get(Supply) >= bonus)
			game.set(Supply, game.get(Supply) - bonus);
		else
			need_stop = true;
		break;
	case ZeroSupplyIfNot:
		if(!game.get(Supply))
			need_stop = true;
		else
			game.set(Supply, 0);
		break;
	case ZeroRerollIfNot:
		if(!game.get(Supply))
			need_stop = true;
		else
			game.set(Reroll, 0);
		break;
	case FullThrottle:
		sail_ship(bonus);
		break;
	case TradeFriend:
		game.tradefriend();
		break;
	case Sail:
		last_tile = sail_next_hexagon();
		clear_message();
		break;
	case ZeroCounters:
		variables.clear();
		break;
	case PenaltyA: case PenaltyB: case PenaltyC: case PenaltyD:
		last_value -= variables.get(v - PenaltyA);
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
		last_value = variables.get(last_counter);
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
	case IfNotSail:
		if(need_sail)
			need_stop = true;
		break;
	case IfVisit:
		// Nothing, this operatior just condition
		break;
	case IfEqual:
		if(last_value != bonus)
			need_stop = true;
		break;
	case IfZeroForward:
		if(!last_value)
			apply_forward(bonus);
		break;
	case IfNonZeroForward:
		if(!last_value)
			apply_forward(bonus);
		break;
	case IfChoosedAction:
		if(bonus > 0) {
			if(!game.ischoosed(bsdata<quest>::source.indexof(find_action(bonus - 1))))
				need_stop = true;
		} else if(bonus < 0) {
			if(game.ischoosed(bsdata<quest>::source.indexof(find_action(-bonus - 1))))
				need_stop = true;
		}
		break;
	case StopActions:
		need_stop_actions = true;
		break;
	case BonusToAll:
		round_skill_bonus += bonus;
		break;
	case Damage:
		if(!bonus)
			bonus = last_value;
		apply_choose(6200, bonus);
		break;
	case ChooseCustom:
		apply_choose(AnswerCustom + bonus, 1);
		break;
	case CheckDanger:
		check_danger();
		break;
	case PlayStars:
		// TODO
		break;
	case MarkVisit:
		if(!bonus)
			bonus = 1;
		category::set(last_quest->index, category::get(last_quest->index) + bonus);
		break;
	case MarkEntry:
		if(bonus >= 0) {
			auto i = AnswerEntry + bonus;
			auto pn = game.getentryname(i);
			if(pn)
				game.information(getnm("AddEntry"), pn);
			game.settag(AnswerEntry + bonus);
		} else {
			auto i = AnswerEntry + bonus;
			auto pn = game.getentryname(i);
			if(pn)
				game.information(getnm("RemoveEntry"), pn);
			game.removetag(AnswerEntry - bonus);
		}
		break;
	case SetVisit:
		if(!bonus)
			category::remove(last_quest->index);
		else
			category::set(last_quest->index, bonus);
		break;
	case IfLast:
		test_last_action();
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

void gamei::apply(variant v) {
	switch(v.type) {
	case Ability: ability_command((ability_s)v.value, v.counter); break;
	case Special: special_command((special_s)v.value, v.counter); break;
	case Card: game.gaintreasure((treasurei*)v.getpointer()); break;
	case Goal: game.setgoal((goali*)v.getpointer()); break;
	}
}