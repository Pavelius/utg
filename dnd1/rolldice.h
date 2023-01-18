#include "dice.h"
#include "nameable.h"

#pragma once

enum dice_s : unsigned char {
	D6, D8, D10
};
struct rolldice : nameable {
	dice	value;
};
