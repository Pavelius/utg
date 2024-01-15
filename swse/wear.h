#include "nameable.h"

#pragma once

enum wear_s : unsigned char {
	Backpack, BackpackLast = Backpack + 15,
	Head, Torso, Hands,
};
struct weari : nameable {
};
extern wear_s last_wear;