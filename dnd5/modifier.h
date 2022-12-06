#pragma once
#include "nameable.h"

enum modifier_s : unsigned char {
	Temporary, Permanent,
	Proficient, Immunity, Resist, Vulnerable,
};
struct modifieri : nameable {
};
extern modifier_s modifier;
extern bool permanent_modifier;