#include "damage.h"
#include "interval.h"
#include "variant.h"

#pragma once

struct attacki {
	const char*		id;
	unsigned char	count;
	interval		damage;
	variants		hit;
	char			use_per_day;
	explicit operator bool() const { return count != 0; }
};
struct attackable {
	attacki			attacks[4];
};
extern attacki* last_attack;