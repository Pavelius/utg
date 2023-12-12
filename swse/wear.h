#include "nameable.h"

#pragma once

enum wear_s : unsigned char {
	Backpack, Potion, BackpackLast = Backpack + 15,
	Head, Torso, Backward, Hands, Elbows,
};
struct weari : nameable {
};
extern wear_s last_wear;