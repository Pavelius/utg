#include "answers.h"
#include "condition.h"
#include "collection.h"
#include "draw.h"
#include "formula.h"
#include "game.h"
#include "math.h"
#include "planet.h"
#include "pushvalue.h"
#include "quest.h"
#include "querry.h"
#include "rand.h"
#include "script.h"
#include "ship.h"
#include "stringvar.h"
#include "variable.h"

static answers an;
static int result;
static void* last_choose_result;
static collection<ship> ships;

static void add_header(stringbuilder& sb, const char* name) {
	sb.addn("###%1", name);
	sb.addn("---");
}

static void change_ability(modulen v, int bonus) {
	player->add(v, bonus);
}

template<> void fnscript<modulei>(int index, int bonus) {
	last_module = (modulen)index;
	change_ability(last_module, bonus);
}

template<> void fnscript<planeti>(int index, int bonus) {
	last_planet = bsdata<planeti>::elements + index;
}

template<> void fnscript<shipi>(int index, int bonus) {
	player = bsdata<ship>::addz();
	player->type = index;
	player->homeworld = getbsi(last_planet);
	player->position = last_planet->position;
	player->priority = 21;
	player->state = ShipOnOrbit;
	script_run(player->geti().elements);
	player->update();
}

template<> void ftinfo<ship>(const void* object, stringbuilder& sb) {
	auto p = (ship*)object;
	auto& ei = p->geti();
	add_header(sb, ei.getname());
	sb.addn("%Hull: %1i/%2i", p->hull, p->modules[Hull]);
	if(p->shield)
		sb.addn("%Shield: %1i/%2i", p->shield, p->modules[Shield]);
	sb.addn("%Speed: %1i", p->modules[Engine]);
}

static void make_roll(int bonus) {
	int	rolled[2];
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
	if(!last_quest)
		return;
	auto index = last_quest->index;
	auto pe = bsdata<quest>::end();
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

void quest_run(int index) {
	if(!answers::console)
		return;
	pushvalue push_header(answers::header);
	pushvalue push_image(answers::resid);
	pushvalue push_last(last_quest, quest_find_prompt(index));
	while(last_quest) {
		an.clear();
		apply_header();
		apply_text();
		script_run(last_quest->tags);
		add_quest_answers();
		auto pv = an.choose(0, 0, 1);
		if(!pv)
			break;
		else if(bsdata<quest>::source.have(pv)) {
			last_quest = (quest*)pv; result = last_quest->next;
			script_run(last_quest->tags);
			last_quest = quest_find_prompt(result);
		}
	}
}

static void add_variable(int bonus) {
	last_variable->value += bonus;
}

static void set_variable(int bonus) {
	last_variable->value = bonus;
}

static void pass_hours(int bonus) {
	player->wait(bonus * 60);
}

static void jump_next(int bonus) {
	result = bonus;
}

static bool isplayer() {
	return game.getship()==player;
}

static void move_to(int bonus) {
	player->move(last_planet->position);
}

static void select_route_path_to_planet(int bonus) {
	auto system_id = getbsi(last_system);
	for(auto& e : bsdata<planeti>()) {
		if(e.system != system_id)
			continue;
		if(player->position == e.position)
			continue;
		an.add(&e, e.getname());
	}
}

static void choose_action(actionstaten state) {
	pushvalue interactive(answers::interactive, isplayer());
	auto& ei = bsdata<actionstatei>::elements[state];
	const char* cancel = 0;
	if(ei.cancel_state) {
		cancel = ei.cancel;
		if(!cancel)
			cancel = "Cancel";
		cancel = getnm(cancel);
	}
	last_action = (actioni*)an.choose(getnm("WhatYouWantToDo"), cancel);
	if(!last_action) {
		if(ei.cancel_state)
			player->state = ei.cancel_state;
	}
	an.clear();
}

static void update_planets() {
}

static void add_effect(int bonus) {
	player->add(last_module, player->get(Effect) * bonus);
}

static void set_player(int bonus) {
	game.setship(player);
}

static void set_state(int bonus) {
	player->state = last_action_state;
}

static void select_actions(actionstaten state) {
	for(auto& e : bsdata<actioni>()) {
		if(e.state != state)
			continue;
		an.add(&e, e.getname());
	}
}

static const char* choose_cancel(const char* id) {
	if(!id)
		return 0;
	static char temp[260]; stringbuilder sb(temp);
	sb.add(getnm(id));
	return temp;
}

static const char* choose_title(const char* id) {
	static char temp[260]; stringbuilder sb(temp);
	sb.add("%1Ask", id);
	return getnme(temp);
}

static void choose_action_querry(const char* id, const char* cancel) {
	pushvalue push(answers::interactive, isplayer());
	last_choose_result = an.choose(choose_title(id), choose_cancel(cancel));
	if(!last_choose_result) {
		script_stop();
		return;
	}
	if(bsdata<planeti>::have(last_choose_result))
		last_planet = (planeti*)last_choose_result;
	else if(bsdata<systemi>::have(last_choose_result))
		last_system = (systemi*)last_choose_result;
}

static void choose_action_querry(int bonus) {
	if(!last_action)
		return;
	auto querry = bsdata<querryi>::find(str("Select%1", last_action->id));
	if(querry) {
		an.clear(); querry->proc(bonus);
		choose_action_querry(last_action->id, last_action->cancel);
		if(!last_choose_result)
			return;
	}
	if(last_action)
		script_run(last_action->effect);
}

static void update_ship_enviroment() {
	last_planet = player->getplanet();
	last_system = last_planet ? last_planet->getsystem() : 0;
}

static void apply_action_header() {
	update_ship_enviroment();
	if(!isplayer())
		return;
	static char header_text[260];
	answers::header = 0;
	answers::resid = 0;
	if(last_planet) {
		answers::resid = "Earth";
		if(last_planet->getlandscape()->resid)
			answers::resid = last_planet->getlandscape()->resid;
		stringbuilder sb(header_text); sb.add("%1 %2", getnm("Planet"), last_planet->getname());
		answers::header = header_text;
	}
}

static void update_player_action(actionstaten state) {
	pushvalue resid(answers::resid);
	pushvalue header(answers::header);
	apply_action_header();
	an.clear();
	select_actions(state);
	choose_action(state);
	choose_action_querry(0);
}

static void update_each_day() {
	static auto counter = timeable::getround();
	if(timeable::getround() >= counter) {
		counter += timeable::rday;
		for(auto& e : bsdata<ship>())
			e.recover();
	}
}

static void update_ships() {
	ships.clear();
	auto system_id = player->system;
	for(auto& e : bsdata<ship>()) {
		if(e.system != system_id)
			continue;
		if(!player->cansee(e))
			continue;
		ships.add(&e);
	}
}

static void update_order() {
	pushvalue push_ship(player);
	for(auto& e : bsdata<ship>()) {
		player = &e;
		update_ships();
		if(player->ismoving())
			player->domove();
		else
			update_player_action(player->state);
	}
}

void play_player_turn() {
	while(draw::ismodal()) {
		update_each_day();
		update_order();
		moveable::dowait();
	}
}

static void shoot_ships() {

}

static void planet_text(stringbuilder& sb) {
	if(!last_planet)
		sb.add(getnm("NoPlanet"));
	else
		sb.add(last_planet->getname());
}

static void ship_text(stringbuilder& sb) {
	if(!player)
		sb.add(getnm("NoShip"));
}

void stringbuilder_proc(stringbuilder& sb, const char* identifier) {
	if(stringvar_identifier(sb, identifier))
		return;
	if(locale_identifier(sb, identifier))
		return;
	default_string(sb, identifier);
}

BSDATA(stringvari) = {
	{"Planet", planet_text},
};
BSDATAF(stringvari)
BSDATA(querryi) = {
	{"SelectRoutePathToPlanet", select_route_path_to_planet},
};
BSDATAF(querryi)
BSDATA(script) = {
	{"AddEffect", add_effect},
	{"MoveTo", move_to},
	{"Next", jump_next},
	{"PassHours", pass_hours},
	{"Roll", roll},
	{"SetPlayer", set_player},
	{"SetState", set_state},
	{"SetVariable", set_variable},
	{"Variable", add_variable},
};
BSDATAF(script)