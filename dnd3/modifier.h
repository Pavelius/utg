#include "nameable.h"

#pragma once

enum modifier_s : unsigned char {
	Current, Permanent,
	Resist, Immune, Vulnerable
};
struct modifieri : nameable {
};
extern modifier_s modifier;