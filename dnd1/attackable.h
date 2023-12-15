#include "damage.h"
#include "dice.h"

#pragma once

struct attacki {
	unsigned char	count;
	dice			damage;
	explicit operator bool() const { return count != 0; }
};
struct attackable {
	attacki			attacks[4];
};
extern attacki* last_attack;