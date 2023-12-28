#pragma once

#include "nameable.h"

enum reaction_s : unsigned char {
	UnknownReaction,
	Hostile, Unfriendly, NeutralReaction, Indifferent, Friendly
};
struct reactioni : nameable {
};
extern reaction_s reaction;