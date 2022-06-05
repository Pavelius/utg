#include "main.h"

using namespace pathfind;

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

static void choose_step(const char* id) {
	auto push_last = playeri::last;
	playeri::last = game.active;
	choosestep::run(id);
	playeri::last = push_last;
}

static void strategy_phase() {
	for(auto p : game.players) {
		if(!p->strategy) {
			game.active = p;
			choose_step("ChooseStrategy");
		}
	}
}

static void action_phase() {
	auto need_repeat = true;
	while(need_repeat) {
		need_repeat = false;
		for(auto p : game.players) {
			if(p->pass_action_phase)
				continue;
			game.active = p;
			choose_step("ChooseAction");
			need_repeat = true;
		}
	}
}

static void score_objectives() {
}

static void reveal_public_objective() {
}

static void draw_action_cards() {
}

static void remove_command_tokens() {
	for(auto& e : bsdata<systemi>())
		e.activated.clear();
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

void gamei::play() {
	do {
		strategy_phase();
		action_phase();
		status_phase();
		agenda_phase();
	} while(!isvictory());
}