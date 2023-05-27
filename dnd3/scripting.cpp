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

BSDATA(script) = {
	{"Divide", divide},
	{"Number", number},
	{"Multiply", multiply},
	{"AddRandom", add_random},
	{"SetParam", set_param}
};
BSDATAF(script)