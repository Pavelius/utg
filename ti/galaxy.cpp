#include "main.h"

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

static void assign_prototypes() {
	for(auto p : game.origin_players) {
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
	game.origin_players.clear();
	for(auto& e : bsdata<playeri>())
		game.origin_players.add(&e);
	playeri::human = game.origin_players[2];
	game.origin_players.shuffle();
	game.players = game.origin_players;
}

static void determine_speaker() {
	game.speaker = game.players.data[rand() % game.players.count];
}

static void set_control(playeri* player, systemi* system) {
	for(auto& e : bsdata<planeti>()) {
		if(e.location == system)
			e.player = player;
	}
}

static void assign_startup() {
	for(auto p : game.players)
		p->assign(p->startup);
}

static void prepare_players() {
	for(auto p : game.players) {
		auto home = p->gethome();
		set_control(p, home);
		p->indicators[TacticToken] = 3;
		p->indicators[FleetToken] = 3;
		p->indicators[StrategyToken] = 2;
		home->placement(p->troops, p);
	}
}

static void prepare_finish() {
	player = playeri::human;
}

static void clear_galaxy() {
	for(auto& e : bsdata<systemi>())
		e.index = pathfind::Blocked;
}

static void assign_starting_positions() {
	auto index = 0;
	auto& positions = galaxy6.start;
	for(auto p : game.players) {
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

void gamei::prepare() {
	clear_galaxy();
	assign_factions();
	assign_prototypes();
	determine_speaker();
	assign_startup();
	prepare_players();
	assign_starting_positions();
	create_galaxy();
	prepare_finish();
	prepareui();
}