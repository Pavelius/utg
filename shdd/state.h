#include "nameable.h"
#include "flagable.h"

#pragma once

enum state_s : unsigned char {
	NoDexBonusAC,
};
struct statei : nameable {
};
struct stateable : flagable<4> {};