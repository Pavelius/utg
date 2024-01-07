#include "nameable.h"

#pragma once

enum modifier_s : unsigned char {
	Current, Permanent, Calculation,
	Resist, Immune, Vulnerable
};
struct modifieri : nameable {
};
extern modifier_s modifier;