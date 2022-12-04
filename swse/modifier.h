#include "nameable.h"

#pragma once

enum modifier_s : unsigned char {
	NoModifier, Permanent, ItemProficiency,
};
struct modifieri : nameable {
};
extern modifier_s modifier;
