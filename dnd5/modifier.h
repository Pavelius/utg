#include "nameable.h"

#pragma once

enum modifier_s : unsigned char {
	NoModifier, Permanent,
	Allowed, Proficient, Immunity, Resist, Vulnerable,
};
struct modifieri : nameable {
};
extern modifier_s modifier;
extern bool permanent_modifier;