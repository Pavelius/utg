#include "ability.h"
#include "script.h"
#include "statable.h"

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
	{"SetParam", set_param}
};
BSDATAF(script)