#include "ability.h"
#include "script.h"

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

BSDATA(script) = {
	{"Divide", divide},
	{"Number", number},
	{"Multiply", multiply}
};
BSDATAF(script)