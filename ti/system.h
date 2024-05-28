#include "entity.h"
#include "tile.h"
#include "variant.h"

#pragma once

struct systemi : entity {
	playeri*		home;
	char			special_index;
	tile_s			special;
	unsigned short	index;
	explicit operator bool() const;
	planeti*		getbestplanet() const;
	int				getcapacity(bool include_docks = true) const;
	bool			isactivated(const playeri* p) const;
	bool			isplay() const;
	void			limitcapacity();
	bool			movestop() const;
	bool			movethrought() const;
	void			paint() const;
	void			placement(const uniti* unit, playeri* player);
	void			placement(variants source, playeri* player);
	void			setactivate(const playeri* p, bool active);
};
extern systemi* last_system;

void block_enemy(const playeri* player);
void block_move();
void mark_zero_cost(const playeri* player);

systemi* find_system(short unsigned i);
