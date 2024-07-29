#pragma once

#include "crt.h"
#include "nameable.h"

struct sprite;
struct tilei : nameable {
	sprite*			resource;
	bool			not_found;
	sprite*			getres();
};
struct tileseti : nameable {
	sliceu<tilei>	tiles;
};
extern tileseti* last_tileset;

tileseti* find_tileset(const tilei* p);

