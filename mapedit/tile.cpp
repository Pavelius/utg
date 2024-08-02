#include "bsreq.h"
#include "io_stream.h"
#include "tile.h"
#include "stringbuilder.h"
#include "variant.h"

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

const char* szt(const char* format, ...) {
	char temp[512]; stringbuilder sb(temp);
	sb.addv(format, xva_start(format));
	return szdup(temp);
}

void read_tiles() {
	auto push_object = bsreq_file_object;
	auto push_meta = bsreq_file_meta;
	bsreq_file_meta = bsdata<varianti>::find("Tileset")->metadata;
	for(io::file::find file("tiles"); file; file.next()) {
		auto pn = file.name();
		if(pn[0] == '.')
			continue;
		char temp[260]; stringbuilder sb(temp);
		sb.add("tiles/%1/Rules.txt", pn);
		last_tileset = bsdata<tileseti>::add();
		last_tileset->id = szdup(pn);
		last_tileset->tiles.setbegin();
		last_tileset->url = szt("tiles/%1", last_tileset->id);
		bsreq_file_object = last_tileset;
		bsreq::read(temp);
		last_tileset->tiles.setend();
		initialize_resources();
	}
	bsreq_file_meta = push_meta;
	bsreq_file_object = push_object;
	last_tileset = bsdata<tileseti>::find("Redux");
}