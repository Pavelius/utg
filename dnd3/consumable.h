#include "nameable.h"
#include "variant.h"

#pragma once

enum consumable_s : unsigned char {
	PP, GP, SP, CP, Experience, Hits
};
struct consumablei : nameable {
	variant	maximum;
};
struct consumable {
	int		consumables[Hits + 1];
};