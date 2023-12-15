#include "dice.h"
#include "nameable.h"

#pragma once

struct rolldice : nameable {
	dice	value;
};
extern int last_roll, last_roll_raw;