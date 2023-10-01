#include "flagable.h"
#include "nameable.h"

#pragma once

enum state_s : unsigned char {
	Concealment, Flatfooted, Immobilized, Prone, PermanentConditions,
};
typedef flagable<4> statef;
struct statei : nameable {
};