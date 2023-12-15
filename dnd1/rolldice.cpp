#include "crt.h"
#include "rolldice.h"

BSDATA(rolldice) = {
	{"Roll1d6", {1, 6}},
	{"Roll1d8", {1, 8}},
	{"Roll1d10", {1, 10}},
	{"Roll1d12", {1, 12}},
	{"Roll1d20", {1, 20}},
};
BSDATAF(rolldice)

int last_roll_raw, last_roll;