#pragma once

#include "nameable.h"

enum reaction_s : unsigned char {
	Indifferent,
	Hostile, Unfriendly, NeutralReaction, Friendly
};
struct reactioni : nameable {
};
extern reaction_s reaction;