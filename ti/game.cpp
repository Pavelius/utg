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

void gamei::defhandle(trigger_s trigger, void* result) {
}

static void select_planets(const playeri* player, bool iscontrol) {
	querry.clear();
	querry.select(bsdata<planeti>::source);
	querry.match(player, iscontrol);
}

static void add_planets(answers& an) {
	select_planets(playeri::last, true);
	querry.match(Exhaust, false);
	querry.match(game.indicator, true);
	for(auto p : querry)
		an.add(p, getnm("PayAnswer"), getnm(p->id), p->get(game.indicator));
}

static bool apply_pay() {
	if(bsdata<planeti>::have(game.result)) {
		auto p = (planeti*)game.result;
		game.options -= p->get(game.indicator);
		p->exhaust();
	} else
		return false;
	return true;
}

void gamei::pay() {
	game.choose(WhenPay, "PayPrompt", add_planets, apply_pay);
}

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

void gamei::choose(trigger_s trigger, const char* title, fnanswer panswer, fnapplyanswer papply) {
	char temp[260]; gamestringbuilder sb(temp);
	answers an;
	draw::pause();
	while(options > 0) {
		sb.clear();
		sb.add(getnm(title));
		sb.adds(getnm("ChooseOptions"), options);
		an.clear(); panswer(an);
		result = an.choose(temp);
		if(!result)
			break;
		if(!papply())
			game.defhandle(trigger, result);
	}
	draw::pause();
}

static void assign_factions() {
	game.players.clear();
	for(auto& e : bsdata<playeri>())
		game.players.add(&e);
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
		set_control(p, p->gethome());
		p->indicators[TacticToken] = 3;
		p->indicators[FleetToken] = 3;
		p->indicators[StrategyToken] = 2;
	}
}

static void prepare_finish() {
	game.human = game.players.data[0];
	game.active = game.human;
}

void gamei::prepare() {
	assign_factions();
	determine_speaker();
	prepare_players();
	prepare_finish();
}