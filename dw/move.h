#include "ability.h"
#include "flagable.h"

#pragma once

enum move_s : unsigned char {
	HackAndSlash, Volley, DefyDanger, DefyPoison, Defend, SpoutLore, DiscernRealities, Parley,
	Aid, Interfere, LastBreath, Encumbrance,
	MakeCamp, TakeWatch, UndertakeAPerilousJourney, LevelUp, EndOfSession,
	Carouse, Supply, Recover, Recruit, OutstandingWarrants, Bolster,
};
typedef flagable<1 + Carouse / 8> movea;
struct movei {
	const char*		id;
	ability_s		ability;
};
struct moveable {
	movea			moves;
	char			forward;
	bool			is(move_s v) const { return moves.is(v); }
};
