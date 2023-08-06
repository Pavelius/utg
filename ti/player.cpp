#include "entitya.h"
#include "pushvalue.h"
#include "speech.h"
#include "system.h"
#include "planet.h"
#include "player.h"
#include "playera.h"
#include "strategy.h"
#include "troop.h"
#include "unit.h"

playeri* player;
playeri* speaker;
playeri* human_player;

int finale_score;

static int d100() {
	return rand() % 100;
}

bool playeri::ishuman() const {
	return human_player == this;
}

int	playeri::getindex() const {
	return origin_players.find(const_cast<playeri*>(this));
}

void playeri::actv(const char* format, const char* format_param) const {
	if(!answers::console)
		return;
	answers::console->addv(format, format_param);
}

void playeri::add(indicator_s v, int i) {
	auto n0 = get(v);
	auto n1 = n0 + i;
	if(n0 == n1)
		return;
	indicators[v] = n1;
}

void playeri::setcontrol(planeti* planet) {
	if(planet->player != this) {
		planet->player = this;
		draw::information(getnm("SetControlPlanet"),
			getnm(id), getnm(planet->id));
	}
}

systemi* playeri::gethome() const {
	for(auto& e : bsdata<systemi>()) {
		if(e.home == this)
			return &e;
	}
	return 0;
}

bool playeri::canbuild(const uniti* unit) const {
	auto maxcount = unit->abilities[Reinforcement];
	if(maxcount < 50) {
		auto count = getsummary(unit);
		if(count >= maxcount)
			return false;
	}
	return true;
}

int	playeri::getsummary(const uniti* type) const {
	auto result = 0;
	for(auto& e : bsdata<troop>()) {
		if(e.player == this && e.getunit() == type)
			result++;
	}
	return result;
}

int	playeri::getplanetsummary(indicator_s v) const {
	entitya source;
	source.select(bsdata<planeti>::source);
	source.match(this, true);
	source.match(Exhaust, false);
	return source.getsummary(v);
}

int	playeri::getinitiative() const {
	if(!strategy)
		return 0;
	return strategy->initiative;
}

int	playeri::getactioncards() const {
	auto result = 0;
	for(auto& e : bsdata<entity>()) {
		auto p = e.getactioncard();
		if(!p)
			continue;
		if(e.player == this)
			result++;
	}
	return result;
}

int playeri::gettechs() const {
	auto result = 0;
	for(auto i = PlasmaScoring; i <= IntegratedEconomy; i = (tech_s)(i + 1)) {
		if(is(i))
			result++;
	}
	return result;
}

void playeri::assign(variants source) {
	for(auto v : source) {
		if(v.iskind<techi>())
			tech.set(v.value);
	}
}

const uniti* playeri::getunit(int index) const {
	if(index == -1)
		return 0;
	if(index < (int)(sizeof(prototype::units) / sizeof(prototype::units[0]))) {
		auto i = getindex();
		if(i == -1)
			return 0;
		return bsdata<prototype>::elements[i].units + index;
	}
	return bsdata<uniti>::elements + index;
}

void playeri::getadvance(requirement& result) const {
	for(auto i = PlasmaScoring; i <= IntegratedEconomy; i = (tech_s)(i + 1)) {
		if(!is(i))
			continue;
		result.required[bsdata<techi>::elements[i].color - 1]++;
	}
}

void playeri::sayspeech(const char* id) const {
	variant test[] = {id};
	auto p = speech::find(test);
	if(p)
		actv(p->name, 0);
}

bool playeri::askv(const char* header_id, const char* format, const char* format_param) const {
	pushvalue push_header(answers::header, getnm(header_id));
	pushvalue push_resid(answers::resid, id);
	return draw::yesnov(format, format_param);
}

bool playeri::ask(const char* header_id, const char* format, ...) const {
	return askv(header_id, format, xva_start(format));
}

void playeri::sayv(const char* format, const char* format_param) const {
	pushvalue push_header(answers::header, getnm(id));
	pushvalue push_resid(answers::resid, id);
	answers::console->addv(format, format_param);
	draw::pause();
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

void players_sort_by_initiative() {
	qsort(players.data, players.count, sizeof(players.data[0]), compare_players);
}

int getrate(indicator_s need, indicator_s currency, int count) {
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