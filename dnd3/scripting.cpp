#include "ability.h"
#include "consumable.h"
#include "creature.h"
#include "material.h"
#include "modifier.h"
#include "script.h"
#include "statable.h"

template<> void fnscript<abilityi>(int index, int value) {
	player->basic.abilities[index] += value;
}

template<> void fnscript<consumablei>(int index, int value) {
	player->consumables[index] += value;
}

template<> void fnscript<materiali>(int index, int value) {
	if(value >= 0) {
		switch(modifier) {
		case Resist: player->resistance |= FG(index); break;
		case Immune: player->immunity |= FG(index); break;
		case Vulnerable: player->vulnerable |= FG(index); break;
		}
	} else {
		switch(modifier) {
		case Resist: player->resistance &= ~FG(index); break;
		case Immune: player->immunity &= ~FG(index); break;
		case Vulnerable: player->vulnerable &= ~FG(index); break;
		}
	}
}

static int getmaximum(variant v) {

}

static void add_random(int counter) {
	if(!counter)
		return;
	ability_result += xrand(0, counter);
}

static void number(int counter) {
	ability_result += counter;
}

static void divide(int counter) {
	if(counter)
		ability_result /= counter;
}

static void multiply(int counter) {
	if(counter)
		ability_result *= counter;
}

static void set_param(int counter) {
	if(counter<sizeof(result_param)/sizeof(result_param[0]))
		result_param[counter] = ability_result;
}

static bool in_party(const creature* p) {
	for(auto pv : party) {
		if(pv == p)
			return true;
	}
	return false;
}

static void shrink_party() {
	auto ps = party;
	auto pb = party;
	auto pe = pb + sizeof(party) / sizeof(party[0]);
	for(; pb < pe; pb++) {
		if(!(*pb))
			continue;
		*ps++ = *pb;
	}
	while(ps < pe)
		*ps++ = 0;
}

void join_party(int bonus) {
	if(bonus >= 0) {
		if(in_party(player))
			return;
		for(auto& pv : party) {
			if(!pv) {
				pv = player;
				return;
			}
		}
	} else {
		for(auto& pv : party) {
			if(pv==player) {
				pv = 0;
				shrink_party();
				return;
			}
		}
	}
}

BSDATA(script) = {
	{"AddRandom", add_random},
	{"Divide", divide},
	{"JoinParty", join_party},
	{"Number", number},
	{"Multiply", multiply},
	{"SetParam", set_param}
};
BSDATAF(script)