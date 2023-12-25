#include "damage.h"
#include "interval.h"
#include "nameable.h"
#include "variant.h"

#pragma once

struct attacki : nameable {
	unsigned char	number;
	interval		damage;
	variants		hit;
	explicit operator bool() const { return number != 0; }
	constexpr bool	isweapon() const { return number != 0; }
};
struct attackable {
	attacki			attacks[4];
};
extern attacki* last_attack;