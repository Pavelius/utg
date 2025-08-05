#include "flagable.h"
#include "nameable.h"

#pragma once

struct spelli : nameable {
	char level;
	char classes[16];
};
struct spellable {
	char	slots[10];
	flagable<16> spells_knows, spells;
};