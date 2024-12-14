#include "ability.h"
#include "dice.h"
#include "flagable.h"

#pragma once

enum moven : unsigned char;
typedef flagable<4, unsigned> movea;
struct movei {
	const char*	id;
	abilityn	ability;
};
struct moveable {
	movea	moves; // Allowed moves
	char	forward;
	bool	is(moven v) const { return moves.is(v); }
};

extern dice	player_damage, enemy_damage;
