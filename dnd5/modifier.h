#pragma once
#include "nameable.h"

enum modifier_s : unsigned char {
	Proficient,
	Immunity, Resist, Vulnerable,
};
struct modifieri : nameable {
};
extern modifier_s modifier;