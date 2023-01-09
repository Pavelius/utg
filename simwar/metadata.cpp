#include "bsreq.h"
#include "building.h"
#include "player.h"
#include "script.h"
#include "unit.h"
#include "variant.h"

NOBSDATA(point)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)
BSDATAC(buildingi, 128)
BSDATAC(building, 1024)
BSDATAC(landscapei, 32)
BSDATAC(playeri, 16)
BSDATAC(provincei, 128)
BSDATAC(troop, 1024)
BSDATAC(uniti, 48)

BSMETA(buildingi) = {
	BSREQ(id),
	BSDST(effect, costi),
	BSREQ(upgrade),
	BSDST(upkeep, costi),
	{}};
BSMETA(costi) = {
	BSREQ(id),
	{}};
BSMETA(landscapei) = {
	BSREQ(id),
	BSDST(effect, costi),
	BSDST(upkeep, costi),
	{}};
BSMETA(playeri) = {
	BSREQ(id),
	BSDST(resources, costi),
	BSDST(income, costi),
	BSREQ(start),
	{}};
BSMETA(point) = {
	BSREQ(x), BSREQ(y),
	{}};
BSMETA(provincei) = {
	BSREQ(id),
	BSREQ(owner),
	BSREQ(position),
	BSREQ(landscape),
	BSDST(income, costi),
	{}};
BSMETA(script) = {
	BSREQ(id),
	{}};
BSMETA(uniti) = {
	BSREQ(id),
	BSDST(effect, costi),
	BSDST(upkeep, costi),
	{}};
BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VAR(costi, 1), ftstatus<costi>},
	{"Building", VAR(buildingi, 1), 0, 0, fnscript<buildingi>},
	{"Landscape", VAR(landscapei, 1)},
	{"Player", VAR(playeri, 1)},
	{"Province", VAR(provincei, 1), ftstatus<provincei>},
	{"Script", VAR(script, 1), 0, 0, fnscript<script>},
	{"Unit", VAR(uniti, 1), 0, 0, fnscript<uniti>},
};
BSDATAF(varianti)