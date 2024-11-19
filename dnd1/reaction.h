#pragma once

#include "nameable.h"

enum reactionn : unsigned char {
	Indifferent,
	Hostile, Unfriendly, Friendly, Helpful, Party
};
struct reactioni : nameable {
};
extern reactionn reaction;