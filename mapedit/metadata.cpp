#include "bsreq.h"
#include "crt.h"
#include "tile.h"
#include "variant.h"
#include "widget.h"

BSDATAC(mapperi, 64)
BSDATAC(tilei, 128)
BSDATAC(tileseti, 32)

NOBSDATA(point)

BSMETA(mapperi) = {
	BSREQ(id),
	BSREQ(indecies), BSREQ(maximum),
	{}};
BSMETA(point) = {
	BSREQ(x), BSREQ(y),
	{}};
BSMETA(tilei) = {
	BSREQ(id),
	BSREQ(border),
	BSREQ(body),
	{}};
BSMETA(tileseti) = {
	BSREQ(id),
	BSREQ(size),
	BSREQ(avatar_tile),
	{}};

BSDATA(varianti) = {
	{"Mapper", VAR(mapperi, 1)},
	{"Tile", VAR(tilei, 1)},
	{"Tileset", VAR(tileseti, 1)},
	{"Widget", VAR(widget, 1)},
};
BSDATAF(varianti);