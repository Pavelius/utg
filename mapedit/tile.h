#pragma once

#include "crt.h"
#include "nameable.h"
#include "point.h"

struct sprite;
struct tilei : nameable {
	sprite*			resource;
	bool			not_found;
	sprite*			getres();
};
struct tileseti : nameable {
	point			size;
	sliceu<tilei>	tiles;
	int				avatar_tile;
};
extern tileseti* last_tileset;

tileseti* find_tileset(const tilei* p);

