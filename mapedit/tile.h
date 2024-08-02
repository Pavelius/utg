#pragma once

#include "crt.h"
#include "nameable.h"
#include "point.h"

struct sprite;
struct mapperi : nameable {
	int				indecies[64];
	int				maximum;
};
struct tilei : nameable {
	mapperi*		border;
	mapperi*		body;
	sprite*			resource;
	int				avatar;
	tilei*			background;
	bool			not_found;
	sprite*			getres();
};
struct tileseti : nameable {
	point			size;
	sliceu<tilei>	tiles;
	const char*		url;
	int				avatar_tile;
};
extern tileseti* last_tileset;

tileseti* find_tileset(const tilei* p);

