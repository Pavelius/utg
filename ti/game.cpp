#include "main.h"

gamei	game;
int		gamei::options;
void*	gamei::result;

class gamestringbuilder : public stringbuilder {
	void addidentifier(const char* identifier) override {
		if(equal(identifier, "indicator"))
			add(getnm(bsdata<indicatori>::elements[game.indicator].id));
		else
			stringbuilder::addidentifier(identifier);
	}
public:
	gamestringbuilder(const stringbuilder& v) : stringbuilder(v) {}
};

int gamei::rate(indicator_s need, indicator_s currency, int count) {
	auto maximum = playeri::last->get(currency) / count;
	if(!maximum)
		return 0;
	auto pn = getnm(bsdata<indicatori>::elements[need].id);
	auto pc = getnm(bsdata<indicatori>::elements[currency].id);
	char temp[260]; gamestringbuilder sb(temp);
	sb.add(getnm("RatePrompt"), pn, pc);
	answers an;
	for(auto i = 0; i < maximum; i++)
		an.add((void*)i, getnm("RateAnswer"), pn, pc);
	return (int)an.choose(temp);
}

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
	point positions[6] = {{2, 0}, {5, 0}};
	auto index = 0;
	for(auto p : game.players) {
		auto ps = p->gethome();
		if(!ps)
			continue;
		ps->index = h2i(positions[index++]);
	}
}

static void create_galaxy() {
	bsdata<systemi>::elements[0].index = h2i({4, 3});
	bsdata<systemi>::elements[1].index = h2i({3, 0});
	bsdata<systemi>::elements[2].index = h2i({2, 1});
	bsdata<systemi>::find("SupernovaSystem")->index = h2i({3, 1});
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

void gamei::initialize() {
	pathfind::maxcount = hms * hms;
	pathfind::maxdir = 6;
}