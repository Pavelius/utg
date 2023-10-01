#include "answers.h"
#include "filter.h"
#include "player.h"
#include "playera.h"
#include "pushvalue.h"
#include "script.h"
#include "list.h"

template<> void fnscript<filteri>(int value, int counter) {
	auto& ei = bsdata<filteri>::elements[value];
	ei.action(ei.proc, counter);
}

static void addan(entity& e) {
	an.add(&e, e.getname());
}

static bool no_player(const void* object) {
	auto p = (entity*)object;
	return p->player == 0;
}

static void choose_strategy(fnvisible proc, int bonus) {
	auto keep = (bonus >= 0);
	an.clear();
	for(auto& e : bsdata<strategyi>()) {
		if(proc(&e) != keep)
			continue;
		addan(e);
	}
	last_strategy = (strategyi*)an.choose();
}

static void select_players(fnvisible proc, int bonus) {
	auto keep = (bonus >= 0);
	if(proc) {
		for(auto& e : bsdata<playeri>()) {
			if(proc(&e) != keep)
				continue;
			players.add(&e);
		}
	} else {
		for(auto& e : bsdata<playeri>())
			players.add(&e);
	}
}

BSDATA(filteri) = {
	{"ChooseStrategy", no_player, choose_strategy},
	{"SelectPlayersByKing", 0, select_players},
};
BSDATAF(filteri)