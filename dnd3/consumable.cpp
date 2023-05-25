#include "consumable.h"
#include "crt.h"

BSDATA(consumablei) = {
	{"PP"},
	{"GP"},
	{"SP"},
	{"CP"},
	{"Experience"},
	{"Hits"},
};
assert_enum(consumablei, Hits)