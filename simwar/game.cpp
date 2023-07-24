#include "game.h"
#include "pushvalue.h"
#include "script.h"
#include "randomizer.h"

gamei game;

inline int d100() {
	return rand() % 100;
}

static void random_explore() {
	for(auto& e : bsdata<provincei>()) {
		auto size = e.landscape->effect[Size] + e.income[Size];
		auto minimum = size * 3;
		auto maximum = minimum + size * 10;
		e.explore(xrand(minimum, maximum));
	}
}

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

void gamei::initialize() {
	year = 1410;
	random_explore();
	neutral_dwelvers();
}