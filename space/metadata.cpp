#include "bsreq.h"
#include "game.h"
#include "planet.h"
#include "tag.h"
#include "variant.h"

NOBSDATA(variants)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSDATAC(landscapei, 32)
BSDATAC(planeti, 512)
BSDATAC(systemi, 32)

BSMETA(landscapei) = {
	BSREQ(id),
	BSDST(resources, planetri),
	{}};
BSMETA(tagi) = {
	BSREQ(id),
	{}};
BSMETA(planeti) = {
	BSREQ(id),
	BSREQ(system),
	{}};
BSMETA(planetri) = {
	BSREQ(id),
	{}};
BSMETA(systemi) = {
	BSREQ(id),
	{}};

BSDATA(varianti) = {
	{"NoVariant"},
	{"Landscape", VAR(landscapei, 1)},
	{"Planet", VAR(planeti, 1)},
	{"PlanetResource", VAR(planetri, 1)},
	{"System", VAR(systemi, 1)},
	{"Tag", VAR(tagi, 1)},
};
BSDATAF(varianti)