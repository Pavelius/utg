#pragma once
#include "flagable.h"
#include "nameable.h"

enum state_s : unsigned char {
	Concealment, Immobilized, Prone, PermanentConditions,
};
typedef flagable<4> statef;
struct statei : nameable {
};