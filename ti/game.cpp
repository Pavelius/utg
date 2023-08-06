#include "main.h"
#include "pushvalue.h"
#include "player.h"
#include "script.h"
#include "system.h"

using namespace pathfind;

gamei game;
decka actioncards;

int gamei::rate(indicator_s need, indicator_s currency, int count) {
	auto maximum = player->get(currency) / count;
	if(!maximum)
		return 0;
	auto pn = getnm(bsdata<indicatori>::elements[need].id);
	auto pc = getnm(bsdata<indicatori>::elements[currency].id);
	char temp[260]; stringbuilder sb(temp);
	sb.add(getnm("RatePrompt"), pn, pc);
	answers an;
	for(auto i = 0; i < maximum; i++)
		an.add((void*)i, getnm("RateAnswer"), pn, pc);
	return (int)an.choose(temp);
}

static point getdirection(point hex, int direction) {
	static point evenr_directions[2][6] = {
		{{+1, 0}, {1, -1}, {0, -1}, {-1, 0}, {0, +1}, {+1, +1}},
		{{+1, 0}, {0, -1}, {-1, -1}, {-1, 0}, {-1, +1}, {0, +1}},
	};
	auto parity = hex.y & 1;
	auto offset = evenr_directions[parity][direction];
	return hex + offset;
}

static indext getdirection(indext index, int direction) {
	if(index == Blocked)
		return Blocked;
	auto hex = getdirection(i2h(index), direction);
	if(hex.x < 0 || hex.y < 0 || hex.x >= hms || hex.y >= hms)
		return Blocked;
	return h2i(hex);
}

void gamei::initialize() {
	pathfind::maxcount = hms * hms;
	pathfind::maxdir = 6;
	pathfind::to = getdirection;
}

static void strategy_phase() {
	pushvalue push_interactive(answers::interactive, false);
	pushvalue push_resid(answers::resid);
	pushvalue push_header(answers::header);
	pushvalue push_player(player);
	for(auto p : game.players) {
		player = p;
		answers::header = player->getname();
		answers::resid = player->getid();
		script::run("FocusHomeSystem", 0);
		if(!player->strategy)
			script::run("ChooseStrategy", 0);
	}
	game.sortbyinitiative();
}

static void score_objectives() {
}

static void reveal_public_objective() {
}

static void draw_action_cards() {
}

static void remove_command_tokens() {
	for(auto& e : bsdata<systemi>()) {
		e.set(Player1, false);
		e.set(Player2, false);
		e.set(Player3, false);
		e.set(Player4, false);
		e.set(Player5, false);
		e.set(Player6, false);
	}
}

static void ready_cards() {
	for(auto& e : bsdata<planeti>())
		e.flags.remove(Exhaust);
}

static void repair_units() {
}

static void return_strategic_cards() {
	for(auto p : game.players) {
		p->use_strategy = false;
		p->pass_action_phase = false;
		p->strategy = 0;
	}
}

static void agenda_phase() {
}

static void status_phase() {
	score_objectives();
	reveal_public_objective();
	draw_action_cards();
	remove_command_tokens();
	return_strategic_cards();
	repair_units();
	ready_cards();
}

static bool isvictory() {
	return false;
}

void gamei::limitcapacity() {
	for(auto& e : bsdata<systemi>()) {
	}
}

static void action_phase() {
	auto need_repeat = true;
	auto push_player = player;
	auto push_header = answers::header;
	while(need_repeat) {
		need_repeat = false;
		for(auto p : game.players) {
			if(p->pass_action_phase)
				continue;
			player = p;
			answers::header = player->getname();
			script::run("ChooseAction", 0);
			need_repeat = true;
		}
	}
	answers::header = push_header;
	player = push_player;
}

void gamei::play() {
	updatecontrol();
	updateui();
	while(!isvictory()) {
		strategy_phase();
		action_phase();
		status_phase();
		agenda_phase();
	};
}

void gamei::updatecontrol() {
	for(auto& e : bsdata<systemi>()) {
		if(!e)
			continue;
		e.player = 0;
	}
	for(auto& e : bsdata<troop>()) {
		if(!e)
			continue;
		if(bsdata<systemi>::have(e.location)
			|| bsdata<planeti>::have(e.location)) {
			if(bsdata<systemi>::have(e.location))
				e.location->player = e.player;
			else
				e.location->player = e.player;
		}
	}
}

static int compare_players(const void* v1, const void* v2) {
	auto p1 = *((playeri**)v1);
	auto p2 = *((playeri**)v2);
	auto i1 = p1->getinitiative();
	auto i2 = p2->getinitiative();
	if(i1 != i2)
		return i1 - i2;
	return getbsi(p1) - getbsi(p2);
}

void gamei::sortbyinitiative() {
	qsort(players.data, players.count, sizeof(players.data[0]), compare_players);
}