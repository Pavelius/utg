#include "answers.h"
#include "condition.h"
#include "console.h"
#include "collection.h"
#include "draw.h"
#include "list.h"
#include "math.h"
#include "pushvalue.h"
#include "quest.h"
#include "questlist.h"
#include "querry.h"
#include "rand.h"
#include "script.h"
#include "ship.h"
#include "speech.h"
#include "stringvar.h"

static answers an;
static int result;
static void* last_choose_result;
static collection<ship> ships;
static int last_value;
static int target_distance;

static void set_opponent(int bonus) {
	opponent = player;
}

static void run_script(const char* id, const char* postfix) {
	char temp[260]; stringbuilder sb(temp);
	if(id)
		sb.add(id);
	if(postfix)
		sb.add(postfix);
	auto pi = bsdata<listi>::find(temp);
	if(pi)
		script_run(pi->elements);
}

static void change_ability(modulen v, int bonus) {
	player->modules[v] += bonus;
	if(player->modules[v] <= 0) {
	}
}

template<> void fnscript<modulei>(int index, int bonus) {
	last_module = (module_s)index;
	change_ability(last_module, bonus);
}

template<> void fnscript<shipi>(int index, int bonus) {
	player = bsdata<ship>::addz();
	player->type = index;
	script_run(player->geti().geti().elements);
	script_run(player->geti().elements);
	player->update();
}

static const quest* find_prompt(const sliceu<quest>& elements, short index) {
	for(auto& e : elements) {
		if(e.index != index)
			continue;
		if(e.isanswer())
			continue;
		if(!script_allow(e.tags))
			continue;
		return &e;
	}
	return 0;
}

static const quest* find_prompt(short index) {
	if(!last_questlist)
		return 0;
	return find_prompt(last_questlist->elements, index);
}

template<> void fnscript<questlist>(int index, int bonus) {
	last_quest = find_prompt(bsdata<questlist>::elements[index].elements, bonus);
}

static int d12() {
	return 1 + (rand() % 12);
}

static void make_roll(int bonus) {
	int	rolled[2] = {};
	char temp[260]; stringbuilder sb(temp);
	if(player->get(Problem) > 0) {
		change_ability(Problem, -1);
		rolled[1] = d12();
	} else
		rolled[1] = 0;
	auto need_reroll = true;
	while(true) {
		if(need_reroll) {
			rolled[0] = d12();
			if(rolled[1] && rolled[0] > rolled[1])
				iswap(rolled[0], rolled[1]);
			result = rolled[0] + bonus;
			need_reroll = false;
		}
		sb.clear();
		if(rolled[1])
			sb.add(getnm("YouRolledProblem"), rolled[0], rolled[1], bonus, result);
		else
			sb.add(getnm("YouRolled"), rolled[0], rolled[1], bonus, result);
		an.clear();
		if(player->get(Insight) > 0)
			an.add(bsdata<modulei>::elements + Insight, getnm("UseInside"));
		auto p = an.choose(temp, getnm("AcceptResult"), 1);
		if(!p)
			break;
		if(bsdata<modulei>::have(p)) {
			change_ability((modulen)((modulei*)p - bsdata<modulei>::elements), -1);
			need_reroll = true;
			continue;
		}
	}
}

static void add_quest_answers() {
	last_questlist = find_quest(last_quest);
	if(!last_quest || !last_questlist)
		return;
	auto index = last_quest->index;
	auto pe = last_questlist->elements.end();
	for(auto pa = last_quest + 1; pa < pe; pa++) {
		if(pa->index != index)
			continue;
		if(!pa->isanswer())
			continue;
		if(!script_allow(pa->tags))
			continue;
		an.add(pa, pa->text);
	}
}

static void apply_header() {
	if(!last_quest)
		return;
	auto pn = last_quest->getheader();
	if(pn)
		answers::header = pn;
	pn = last_quest->getimage();
	if(pn)
		answers::resid = pn;
}

static void apply_text() {
	if(last_quest && last_quest->text) {
		answers::console->clear();
		answers::console->add(last_quest->text);
	}
}

static void apply_script() {
	result = 0;
	if(!last_quest)
		return;
	result = last_quest->next;
	script_run(last_quest->tags);
}

static void choose_quest_result() {
	if(!last_quest)
		return;
	auto index = last_quest->index;
	auto pe = bsdata<quest>::end();
	auto pr = last_quest + 1;
	for(auto pa = pr; pa < pe; pa++) {
		if(pa->index != index)
			continue;
		if(!pa->isanswer())
			break;
		pr = pa;
		if(result <= pr->next)
			break;
	}
	last_quest = pr;
}

static void roll(int bonus) {
	make_roll(player->get(last_module) + bonus);
	choose_quest_result();
	apply_text();
	apply_script();
	pause();
	last_quest = quest_find_prompt(result);
	apply_header();
	apply_text();
}

static void ability_script_run() {
	if(player->hull <= 0)
		run_script("Hull", "DropZero");
}

void run_current_quest() {
	if(!answers::console)
		return;
	last_questlist = find_quest(last_quest);
	pushvalue push_header(answers::header);
	pushvalue push_image(answers::resid);
	while(last_quest) {
		an.clear();
		apply_header();
		apply_text();
		script_run(last_quest->tags);
		add_quest_answers();
		if(an) {
			auto pv = an.choose(0, 0, 1);
			answers::console->clear();
			if(!pv)
				break;
			else if(bsdata<quest>::source.have(pv)) {
				last_quest = (quest*)pv; result = last_quest->next;
				script_run(last_quest->tags);
				last_quest = find_prompt(result);
			}
		} else {
			pause();
			last_quest = 0;
		}
		ability_script_run();
	}
}

static void add_value(int bonus) {
	last_value += bonus;
}

static void set_value(int bonus) {
	last_value = bonus;
}

static void jump_next(int bonus) {
	result = bonus;
}

static void add_effect(int bonus) {
	last_modules->modules[last_module] += last_modules->modules[Effect] * bonus;
}

static void printn(const char* format, ...) {
	add_console("", format, xva_start(format));
}

static void print_value(const char* id, int n) {
	auto pi = speech_find(id);
	if(!pi)
		return;
	printn(speech_get(pi, n));
}

static void add_target_distance(int bonus) {
	target_distance += bonus;
	if(answers::interactive)
		print_value("TargetDistance", target_distance);
}

static void fire_weapon(modulen v) {
	auto& ei = bsdata<modulei>::elements[v];
	auto total_shoots = player->modules[v];
	if(!total_shoots)
		return;
	auto chance_hit = get_hit_chance(v, target_distance, opponent->get(Engine));
	if(chance_hit <= 0)
		return;
	auto hits = roll_hits(total_shoots, chance_hit);
	auto damage = roll_damage(hits, ei.damage.minimum(), ei.damage.maximum(), get_critical(v), get_critical_multiplayer(v));
	if(hits)
		printn(getnm("ApplyDamage"), ei.getname(), hits, damage);
	else
		printn(getnm("MissWeapon"), ei.getname(), hits, damage);
	opponent->hull -= damage;
}

static void fire_weapons(int bonus) {
	for(auto i = ShardCannons; i <= RocketLaunchersIII; i = (modulen)(i + 1))
		fire_weapon(i);
}

static void change_opponents(int bonus) {
	iswap(player, opponent);
}

BSDATA(script) = {
	{"AddEffect", add_effect},
	{"ChangeOpponents", change_opponents},
	{"FireWeapons", fire_weapons},
	{"Inflict", set_inflict},
	{"Next", jump_next},
	{"Roll", roll},
	{"SetCurrent", set_current},
	{"SetPermanent", set_permanent},
	{"SetOpponent", set_opponent},
	{"SetValue", set_value},
	{"TargetDistance", add_target_distance},
	{"Suffer", set_suffer},
	{"Value", add_value},
};
BSDATAF(script)