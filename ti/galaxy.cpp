#include "answers.h"
#include "card.h"
#include "pathfind.h"
#include "planet.h"
#include "player.h"
#include "point.h"
#include "pushvalue.h"
#include "rand.h"
#include "script.h"
#include "strategy.h"
#include "system.h"
#include "unit.h"
#include "troop.h"

typedef adat<systemi*> systema;
using namespace pathfind;

namespace {
struct pointline {
	point				position;
	char				count;
};
struct galaxymap {
	slice<pointline>	tiles;
	point				start[6];
};
}

static pointline players6[] = {
	{{2, 0}, 2},
	{{1, 1}, 5},
	{{0, 2}, 6},
	{{1, 3}, 2},
	{{4, 3}, 2},
	{{0, 4}, 6},
	{{1, 5}, 5},
	{{2, 6}, 2},
};
static galaxymap galaxy6 = {players6, {{1, 0}, {4, 0}, {0, 3}, {6, 3}, {1, 6}, {4, 6}}};

void initialize_decks();

static point getdirection(point hex, int direction) {
	static point evenr_directions[2][6] = {
		{{+1, 0}, {1, -1}, {0, -1}, {-1, 0}, {0, +1}, {+1, +1}},
		{{+1, 0}, {0, -1}, {-1, -1}, {-1, 0}, {-1, +1}, {0, +1}},
	};
	auto parity = hex.y & 1;
	auto offset = evenr_directions[parity][direction];
	return hex + offset;
}

static indext getdirection(short unsigned index, int direction) {
	if(index == Blocked)
		return Blocked;
	auto hex = getdirection(i2h(index), direction);
	if(hex.x < 0 || hex.y < 0 || hex.x >= hms || hex.y >= hms)
		return Blocked;
	return h2i(hex);
}

void initialize_game() {
	pathfind::maxcount = hms * hms;
	pathfind::maxdir = 6;
	pathfind::to = getdirection;
}

static void assign_prototypes() {
	for(auto p : players) {
		if(!p)
			continue;
		auto proto = bsdata<prototype>::elements + p->getindex();
		for(auto& e : bsdata<uniti>()) {
			if(e.race && e.race != p)
				continue;
			auto index = getbsi(&e);
			if(e.replace)
				index = getbsi(e.replace);
			if(index < sizeof(proto->units) / sizeof(proto->units[0]))
				proto->units[index] = e;
		}
	}
}

static void add_player(playeri* p) {
	for(auto& e : players) {
		if(e)
			continue;
		e = p;
		p->player = p;
		break;
	}
}

static int player_count() {
	auto r = 0;
	for(auto p : players) {
		if(p)
			r++;
	}
	return r;
}

static void assign_factions() {
	memset(players, 0, sizeof(players));
	for(auto& e : bsdata<playeri>())
		add_player(&e);
	human_player = players[2];
	zshuffle(players, player_count());
}

static void determine_speaker() {
	speaker = players[rand() % player_count()];
}

static void set_control(playeri* player, systemi* system) {
	for(auto& e : bsdata<planeti>()) {
		if(e.location == system)
			e.player = player;
	}
}

static void assign_startup() {
	for(auto p : players)
		p->assign(p->startup);
}

static void prepare_players() {
	for(auto p : players) {
		auto home = p->gethome();
		set_control(p, home);
		p->indicators[TacticToken] = 3;
		p->indicators[FleetToken] = 3;
		p->indicators[StrategyToken] = 2;
		home->placement(p->troops, p);
	}
}

static void prepare_finish() {
	player = human_player;
	finale_score = 8;
}

static void clear_galaxy() {
	for(auto& e : bsdata<systemi>())
		e.index = pathfind::Blocked;
}

static void assign_starting_positions() {
	auto index = 0;
	auto& positions = galaxy6.start;
	for(auto p : players) {
		auto ps = p->gethome();
		if(!ps)
			continue;
		ps->index = h2i(positions[index++]);
	}
}

static void shuffle(systema& source) {
	zshuffle(source.data, source.count);
}

static void add_systems(systema& result) {
	for(auto& e : bsdata<systemi>()) {
		if(e.home)
			continue;
		if(equal(e.id, "MecatolRexSystem"))
			continue;
		result.add(&e);
	}
}

static void create_galaxy(systema& tiles, galaxymap& source) {
	auto tile_index = 0;
	for(auto& e : source.tiles) {
		auto index = h2i(e.position);
		for(auto i = 0; i < e.count; i++)
			tiles[tile_index++]->index = index++;
	}
	bsdata<systemi>::elements[0].index = h2i({3, 3});
}

static void create_galaxy() {
	systema systems;
	systems.clear();
	add_systems(systems);
	shuffle(systems);
	create_galaxy(systems, galaxy6);
}

void prepare_game() {
	clear_galaxy();
	initialize_decks();
	assign_factions();
	assign_prototypes();
	determine_speaker();
	assign_startup();
	prepare_players();
	assign_starting_positions();
	create_galaxy();
	prepare_finish();
	prepare_game_ui();
	update_control();
	update_ui();
}

static void strategy_phase() {
	pushvalue push_interactive(answers::interactive, true);
	pushvalue push_resid(answers::resid);
	pushvalue push_header(answers::header);
	pushvalue push_player(player);
	for(auto p : players) {
		player = p;
		answers::header = player->getname();
		answers::resid = player->getid();
		script_run("FocusHomeSystem", 0);
		if(!player->getstrategy())
			script_run("ChooseStrategy", 0);
	}
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
	for(auto& e : bsdata<troop>()) {
		if(e.is(RepairSustainDamage) && e.is(Exhaust)) {
			e.set(Exhaust, false);
			if(e.player->ishuman())
				e.status(0, "%1 %-Repaired");
		}
	}
	pause();
}

static void return_strategic_cards() {
	for(auto p : players) {
		p->use_strategy = false;
		p->pass_action_phase = false;
	}
	for(auto& e : bsdata<strategyi>())
		e.player = 0;
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

void choose_action(int bonus);

static void action_phase() {
	pushvalue push_header(answers::header);
	pushvalue push_player(player);
	auto need_repeat = true;
	while(need_repeat) {
		need_repeat = false;
		for(auto p : players) {
			if(p->pass_action_phase)
				continue;
			player = p; answers::header = player->getname();
			if(p->ishuman())
				p->gethome()->focusing();
			choose_action(0);
			need_repeat = true;
		}
	}
}

void play_game() {
	update_control();
	update_ui();
	while(!isvictory()) {
		strategy_phase();
		action_phase();
		status_phase();
		agenda_phase();
	};
}

void update_control() {
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