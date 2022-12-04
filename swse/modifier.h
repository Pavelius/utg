#include "nameable.h"

#pragma once

enum modifiers : unsigned char {
	NoModifier, Permanent,
};
struct modifieri : nameable {
};
extern modifiers modifier;
