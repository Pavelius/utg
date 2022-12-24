#include "bsreq.h"
#include "game.h"
#include "planet.h"
#include "script.h"
#include "tag.h"
#include "variant.h"

NOBSDATA(variants)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSDATAC(landscapei, 32)
BSDATAC(planeti, 512)
BSDATAC(systemi, 32)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(landscapei) = {
	BSREQ(id),
	BSDST(resources, planetri),
	{}};
BSMETA(script) = {
	BSREQ(id),
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
	{"Ability", VAR(abilityi, 1), 0, 0, fnscript<abilityi>},
	{"Landscape", VAR(landscapei, 1)},
	{"Planet", VAR(planeti, 1)},
	{"PlanetResource", VAR(planetri, 1)},
	{"Script", VAR(script, 1), 0, 0, fnscript<script>},
	{"System", VAR(systemi, 1)},
	{"Tag", VAR(tagi, 1)},
};
BSDATAF(varianti)