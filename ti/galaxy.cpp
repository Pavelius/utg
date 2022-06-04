#include "main.h"

using namespace pathfind;

static pointline players6[] = {
	{{3, 0}, 2},
	{{2, 1}, 5},
	{{1, 2}, 6},
	{{2, 3}, 2},
	{{5, 3}, 2},
	{{1, 4}, 6},
	{{2, 5}, 5},
	{{3, 6}, 2},
};

struct galaxymap {
	slice<pointline>	tiles;
	point				start[6];
};

static galaxymap galaxy6 = {players6, {{2, 0}, {5, 0}, {0, 3}, {6, 0}, {2, 6}, {5, 6}}};

static void assign_factions() {
	game.players.clear();
	for(auto& e : bsdata<playeri>())
		game.players.add(&e);
	playeri::human = game.players[0];
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
	game.active = playeri::human;
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
	bsdata<systemi>::elements[0].index = h2i({4, 3});
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
	determine_speaker();
	prepare_players();
	assign_starting_positions();
	create_galaxy();
	prepare_finish();
	prepareui();
}