#include "actioncard.h"
#include "analize.h"
#include "pathfind.h"
#include "planet.h"
#include "player.h"
#include "playera.h"
#include "system.h"
#include "unit.h"

using namespace pathfind;

namespace {
struct pointline {
	point	position;
	char	count;
};
struct galaxymap {
	slice<pointline> tiles;
	point	start[6];
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
	for(auto p : origin_players) {
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

static void assign_factions() {
	origin_players.clear();
	for(auto& e : bsdata<playeri>())
		origin_players.add(&e);
	human_player = origin_players[2];
	origin_players.shuffle();
	players = origin_players;
}

static void determine_speaker() {
	speaker = players.data[rand() % players.count];
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

typedef adat<systemi*> systema;

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

static void add_card(decka& deck, void* parent) {
	auto p = bsdata<entity>::add();
	p->id = (const char*)parent;
	p->flags = 0;
	p->location = 0;
	p->player = 0;
	deck.add(p);
}

static void create_action_card_deck() {
	actioncards.clear();
	for(auto& e : bsdata<actioncard>()) {
		for(auto i = 0; i < e.count; i++)
			add_card(actioncards, &e);
	}
	actioncards.shuffle();
}

void prepare_game() {
	clear_galaxy();
	assign_factions();
	assign_prototypes();
	determine_speaker();
	assign_startup();
	prepare_players();
	assign_starting_positions();
	create_galaxy();
	create_action_card_deck();
	prepare_finish();
	prepare_game_ui();
}