#include "ability.h"
#include "creature.h"
#include "script.h"
#include "statable.h"

template<> void fnscript<abilityi>(int index, int value) {
	player->basic.abilities[index] += value;
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