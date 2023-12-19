#include "damage.h"
#include "interval.h"

#pragma once

struct attacki {
	unsigned char	count;
	interval		damage;
	explicit operator bool() const { return count != 0; }
};
struct attackable {
	attacki			attacks[4];
};
extern attacki* last_attack;