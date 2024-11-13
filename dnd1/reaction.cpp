#include "bsdata.h"
#include "reaction.h"

BSDATA(reactioni) = {
	{"Indifferent"},
	{"Hostile"},
	{"Unfriendly"},
	{"NeutralReaction"},
	{"Friendly"},
};
assert_enum(reactioni, Friendly)

reaction_s reaction;