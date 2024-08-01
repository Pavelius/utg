#pragma once

#include "crt.h"
#include "nameable.h"
#include "point.h"

struct sprite;
struct mapperi : nameable {
	int				indecies[64];
};
struct tilei : nameable {
	mapperi*		border;
	mapperi*		body;
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

