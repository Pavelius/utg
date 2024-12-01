#include "flagable.h"
#include "nameable.h"

#pragma once

enum staten : unsigned char {
	Concealment, Flatfooted, Immobilized, Prone, PermanentConditions, Helpless,
	MeleeEngaged,
};
typedef flagable<4> statef;
struct statei : nameable {
};
extern staten last_state;