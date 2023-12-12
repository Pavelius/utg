#include "crt.h"
#include "wear.h"

wear_s last_wear;

BSDATA(weari) = {
	{"Backpack"},
	{"Potion"},
	{"Backpack2"},
	{"Backpack3"},
	{"Backpack4"},
	{"Backpack5"},
	{"Backpack6"},
	{"Backpack7"},
	{"Backpack8"},
	{"Backpack9"},
	{"Backpack10"},
	{"Backpack11"},
	{"Backpack12"},
	{"Backpack13"},
	{"Backpack14"},
	{"BackpackLast"},
	{"Head"},
	{"Torso"},
	{"Backward"},
	{"Hands"},
	{"Elbows"},
};
assert_enum(weari, Elbows)