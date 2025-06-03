#include "nameable.h"
#include "flagable.h"

#pragma once

enum staten : unsigned char {
	NoDexBonusAC,
	BadSwim, BadStealth,
	NoSwim,
};
struct statei : nameable {
};
struct stateable : flagable<4> {};