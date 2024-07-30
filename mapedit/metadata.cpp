#include "bsreq.h"
#include "crt.h"
#include "tile.h"
#include "variant.h"
#include "widget.h"

BSDATAC(tilei, 128)
BSDATAC(tileseti, 32)

BSMETA(tilei) = {
	BSREQ(id),
	{}};
BSMETA(tileseti) = {
	BSREQ(id),
	{}};

BSDATA(varianti) = {
	{"Tile", VAR(tilei, 1)},
	{"Tileset", VAR(tileseti, 1)},
	{"Widget", VAR(widget, 1)},
};
BSDATAF(varianti);