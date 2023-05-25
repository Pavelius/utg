#include "nameable.h"

#pragma once

enum consumable_s : unsigned char {
	PP, GP, SP, CP, Experience, Hits
};

struct consumablei : nameable {
};
struct consumable {
	int		consumables[Hits + 1];
};
