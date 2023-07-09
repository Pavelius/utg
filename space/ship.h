#include "action.h"
#include "item.h"
#include "moveable.h"

#pragma once

enum shipclass_s : unsigned char {
	Fighter, Fregate, Destroyer, Cruiser, Battleship,
	Carrier,
};

struct planeti;
struct shipclassi : nameable {
};
struct shipi : itemi {
	shipclass_s		kind;
	char			size;
};
struct ship : moveable {
	unsigned short	type, system, homeworld;
	short			hull, shield;
	actionstate_s	state;
	const shipi&	geti() const;
	planeti*		gethomeworld() const;
	planeti*		getplanet() const;
	int				getspeed() const; // Pixels per one seconds
	void			move(point position);
	void			paint() const;
};

void play_player_turn();

extern ship* last_ship;
extern ship* player;
