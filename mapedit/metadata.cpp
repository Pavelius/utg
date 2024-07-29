#include "bsreq.h"
#include "crt.h"
#include "tile.h"
#include "variant.h"

BSDATAC(tilei, 128)
BSDATAC(tileseti, 32)

BSMETA(tilei) = {
	BSREQ(id),
	{}};
BSMETA(tileseti) = {
	BSREQ(id),
	{}};

BSDATA(varianti) = {
	{"Tileset", VAR(tileseti, 1)},
};
BSDATAF(varianti);