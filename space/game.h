#include "timeable.h"

#pragma once

struct ship;

struct gamei : timeable {
	unsigned short	player_id;
	void			clear();
	ship*			getship() const;
	void			setship(const ship* v);
};
extern gamei game;
