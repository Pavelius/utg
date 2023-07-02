#include "item.h"
#include "drawable.h"

#pragma once

struct planeti;
struct shipi : itemi {
};
struct ship : drawable {
	unsigned short	type;
	unsigned short	homeworld;
	planeti*		gethomeworld() const;
	int				getspeed() const; // Pixels per one seconds
	void			move(point position);
	void			paint() const;
};
extern ship* last_ship;
extern ship* player;
