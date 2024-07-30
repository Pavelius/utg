#include "bsreq.h"
#include "io_stream.h"
#include "tile.h"
#include "stringbuilder.h"

tileseti* last_tileset;

sprite* tilei::getres() {
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

static void initialize_resources() {
	for(auto& e : last_tileset->tiles) {
		char temp[260]; stringbuilder sb(temp);
		sb.add("tiles/%1/%2.pma", last_tileset->id, e.id);
		e.resource = (sprite*)loadb(temp);
	}
}

void read_tiles() {
	for(io::file::find file("tiles"); file; file.next()) {
		auto pn = file.name();
		if(pn[0] == '.')
			continue;
		char temp[260]; stringbuilder sb(temp);
		sb.add("tiles/%1/Rules.txt", pn);
		auto pb = (tilei*)bsdata<tilei>::source.end();
		last_tileset = bsdata<tileseti>::end();
		bsreq::read(temp);
		last_tileset->tiles.set(pb, (tilei*)bsdata<tilei>::source.end() - pb);
		initialize_resources();
	}
	last_tileset = bsdata<tileseti>::find("Redux");
}