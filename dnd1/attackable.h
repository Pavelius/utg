#include "damage.h"
#include "interval.h"
#include "variant.h"

#pragma once

struct attacki {
	unsigned char	count;
	interval		damage;
	variants		use;
	explicit operator bool() const { return count != 0; }
};
struct attackable {
	attacki			attacks[4];
};
extern attacki* last_attack;