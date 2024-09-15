#include "game.h"
#include "pushvalue.h"
#include "script.h"
#include "rand.h"
#include "randomizer.h"
#include "list.h"

gamei game;

static void mark_start_provinces() {
	for(auto& e : bsdata<provincei>()) {
		if(e.player)
			e.makewave();
	}
}

static void add_neutral_troops() {
	pushvalue push_player(player);
	pushvalue push_province(province);
	auto ps = bsdata<script>::find("Recruit");
	if(!ps)
		return;
	player = 0;
	for(auto& e : bsdata<provincei>()) {
		if(e.iswater())
			continue;
		if(e.player)
			continue;
		province = &e;
		auto level = e.getcost();
		if(!level)
			continue;
		auto count = level;
		if(count > 8)
			count = 8;
		for(auto i = 0; i < count; i++) {
			if(d100() < 35)
				continue;
			ps->proc(1);
		}
	}
}

static void neutral_dwelvers() {
	clear_wave();
	mark_start_provinces();
	add_neutral_troops();
}

static void add_cards(deck& source, variants& elements) {
	for(auto v : elements) {
		if(v.iskind<cardi>()) {
			auto count = script_count(v.counter);
			for(auto i = 0; i < count; i++)
				source.add(bsdata<cardi>::elements + v.value);
		}
	}
}

static void add_cards(deck& source) {
	auto p = bsdata<listi>::find("StartTactics");
	if(!p)
		return;
	source.clear();
	add_cards(source, p->elements);
	source.shuffle();
}

static void add_player_tactics() {
	add_cards(neutral_tactics);
	for(auto& e : bsdata<playeri>())
		add_cards(e.tactics);
}

void gamei::initialize() {
	year = 1410;
	neutral_dwelvers();
	add_player_tactics();
}