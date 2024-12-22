#pragma once

#include "ability.h"
#include "dice.h"
#include "flagable.h"
#include "nameable.h"

enum moven : unsigned char;
enum movetypen : unsigned char {
	BasicMove,
	StartMove, AdvancedMove
};

struct classi;

typedef flagable<4, unsigned> movea;
struct movei : nameable {
	movetypen	type;
	classi*		playbook;
	abilityn	ability; // If need roll
};
struct movetypei : nameable {
};
struct moveable {
	movea moves; // Allowed moves
	bool is(moven v) const { return moves.is(v); }
};

extern dice	player_damage, enemy_damage;
