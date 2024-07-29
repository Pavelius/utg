#include "tile.h"
#include "stringbuilder.h"

tileseti* last_tileset;

sprite* tilei::getres() {
	if(!resource) {
		if(not_found)
			return 0;
		char temp[260]; stringbuilder sb(temp);
		sb.add("art/%1/%2.pma", last_tileset->getid(), id);
		resource = (sprite*)loadb(temp);
		if(!resource)
			not_found = true;
	}
	return resource;
}

tileseti* find_tileset(const tilei* p) {
	for(auto& e : bsdata<tileseti>()) {
		if(!e.tiles)
			continue;
		auto pb = e.tiles.begin();
		auto pe = e.tiles.end();
		if(p >= pb && p < pe)
			return &e;
	}
	return 0;
}