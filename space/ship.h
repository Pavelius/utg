#include "item.h"
#include "drawable.h"

#pragma once

struct planeti;
struct shipi : itemi {
};
struct ship : drawable {
	unsigned short	type, system, homeworld;
	planeti*		gethomeworld() const;
	planeti*		getplanet() const;
	int				getspeed() const; // Pixels per one seconds
	void			move(point position);
	void			paint() const;
};

void play_player_turn();

extern ship* last_ship;
extern ship* player;
