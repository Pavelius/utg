#include "bsreq.h"
#include "crt.h"
#include "tile.h"
#include "variant.h"
#include "widget.h"

BSDATAC(tilei, 128)
BSDATAC(tileseti, 32)

NOBSDATA(point)

BSMETA(point) = {
	BSREQ(x), BSREQ(y),
	{}};
BSMETA(tilei) = {
	BSREQ(id),
	{}};
BSMETA(tileseti) = {
	BSREQ(id),
	BSREQ(size),
	{}};

BSDATA(varianti) = {
	{"Tile", VAR(tilei, 1)},
	{"Tileset", VAR(tileseti, 1)},
	{"Widget", VAR(widget, 1)},
};
BSDATAF(varianti);