#include "bsreq.h"
#include "main.h"

NOBSDATA(variants)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSDATAC(landscapei, 32)
BSDATAC(planeti, 512)
BSDATAC(systemi, 32)

BSMETA(landscapei) = {
	BSREQ(id),
	BSDST(resources, planet_resourcei),
	{}};
BSMETA(tagi) = {
	BSREQ(id),
	{}};
BSMETA(planeti) = {
	BSREQ(id),
	BSREQ(system),
	{}};
BSMETA(planet_resourcei) = {
	BSREQ(id),
	{}};
BSMETA(systemi) = {
	BSREQ(id),
	{}};

#define VAR(T, KN) bsmeta<T>::meta, bsdata<T>::source_ptr, KN
BSDATA(varianti) = {
	{"NoVariant"},
	{"Landscape", VAR(landscapei, 1)},
	{"Planet", VAR(planeti, 1)},
	{"PlanetResource", VAR(planet_resourcei, 1)},
	{"System", VAR(systemi, 1)},
	{"Tag", VAR(tagi, 1)},
};
BSDATAF(varianti)