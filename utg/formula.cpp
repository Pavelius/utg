#include "bsdata.h"
#include "bsreq.h"
#include "formula.h"
#include "script.h"

int last_number;

static int add_formula(int p1, int p2) {
	return p1 + p2;
}

static int sub_formula(int p1, int p2) {
	return p1 - p2;
}

static int mul_formula(int p1, int p2) {
	return p1 * p2;
}

static int set_formula(int p1, int p2) {
	return p2;
}

template<> void fnscript<formulai>(int value, int bonus) {
	last_number = bsdata<formulai>::elements[value].proc(last_number, bonus);
}

int get_bonus(int bonus) {
	switch(bonus) {
	case 101: return last_number;
	case -101: return -last_number;
	default: return bonus;
	}
}

BSMETA(formulai) = {
	BSREQ(id),
	{}};
BSDATA(formulai) = {
	{"Add", add_formula},
	{"Mul", mul_formula},
	{"Set", set_formula},
	{"Sub", sub_formula},
};
BSDATAF(formulai)