#include "nameable.h"

#pragma once

enum consumable_s : unsigned char {
	CP, SP, GP, Experience,
};
struct consumablei : nameable {
};
struct consumablea {
	int			consumables[Experience + 1];
};