#include "bsdata.h"
#include "reaction.h"

BSDATA(reactioni) = {
	{"Indifferent"},
	{"Hostile"},
	{"Unfriendly"},
	{"Friendly"},
};
assert_enum(reactioni, Friendly)

reactionn reaction;