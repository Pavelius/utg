#include "crt.h"
#include "reaction.h"

BSDATA(reactioni) = {
	{"UnknownReaction"},
	{"Hostile"},
	{"Unfriendly"},
	{"NeutralReaction"},
	{"Indifferent"},
	{"Friendly"},
};
assert_enum(reactioni, Friendly)

reaction_s reaction;