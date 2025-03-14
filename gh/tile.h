#include "point.h"

#pragma once

struct tilei {
	const char*		id;
	point			size; // size of field in tiles
	int				priority;
	point			offset; // offset to upper left tile
	slice<point>	blocks; // blocked squares
	void			creating(point position, bool inverse) const;
	bool			isdecoration() const { return priority >= 40; }
	void			updateui() const;
};
void				creating_tile(tilei* p, point position, bool inverse);
