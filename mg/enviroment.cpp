#include "crt.h"
#include "enviroment.h"

BSDATA(enviroment_typei) = {
	{"Wilderness"},
	{"Room"},
	{"House"},
	{"Street"},
	{"Village"},
	{"Towm"},
	{"City"},
};
assert_enum(enviroment_typei, City)

enviromenti* environment;

