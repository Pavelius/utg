#pragma once

#include "nameable.h"

enum partystatn : unsigned char {
	EffectCount,
};
struct partystati : nameable {
};
struct partyi {
	int abilities[EffectCount + 1];
};
extern partyi party;
