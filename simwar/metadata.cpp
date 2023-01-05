#include "bsreq.h"
#include "building.h"
#include "player.h"
#include "variant.h"

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)
BSDATAC(buildingi, 128)
BSDATAC(building, 1024)
BSDATAC(landscapei, 32)
BSDATAC(playeri, 16)

BSMETA(buildingi) = {
	BSREQ(id),
	BSDST(effect, costi),
	{}};
BSMETA(costi) = {
	BSREQ(id),
	{}};
BSMETA(landscapei) = {
	BSREQ(id),
	BSDST(effect, costi),
	{}};
BSDATA(varianti) = {
	{"NoVariant"},
	{"Building", VAR(buildingi, 1)},
	{"Landscape", VAR(landscapei, 1)},
};
BSDATAF(varianti)