#include "action.h"
#include "item.h"
#include "moveable.h"

#pragma once

struct planeti;
struct shipi : itemi {
};
struct ship : moveable {
	unsigned short	type, system, homeworld;
	actionstate_s	state;
	planeti*		gethomeworld() const;
	planeti*		getplanet() const;
	int				getspeed() const; // Pixels per one seconds
	void			move(point position);
	void			paint() const;
};

void play_player_turn();

extern ship* last_ship;
extern ship* player;
