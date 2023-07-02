#include "item.h"
#include "drawable.h"

#pragma once

struct shipi : itemi {
};
struct ship : drawable {
	unsigned char	type;
	static void		add(const char* id, point position);
	int				getspeed() const; // Pixels per one seconds
	void			move(point position);
	void			paint() const;
};
extern ship* last_ship;
