#include "bsreq.h"
#include "building.h"
#include "player.h"
#include "script.h"
#include "unit.h"
#include "variant.h"

void get_cost_info(const void* object, stringbuilder& sb);

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
BSMETA(combati) = {
	BSREQ(id),
	{}};
BSMETA(landscapei) = {
	BSREQ(id),
	BSDST(effect, costi),
	BSDST(combat, combati),
	{}};
BSMETA(playeri) = {
	BSREQ(id),
	BSDST(cost, costi),
	BSREQ(start),
	{}};
BSMETA(point) = {
	BSREQ(x), BSREQ(y),
	{}};
BSMETA(provincei) = {
	BSREQ(id),
	BSDST(combat, combati),
	BSREQ(owner),
	BSREQ(position),
	BSREQ(landscape),
	{}};
BSMETA(script) = {
	BSREQ(id),
	{}};
BSMETA(uniti) = {
	BSREQ(id),
	BSDST(cost, costi),
	BSDST(combat, combati),
	BSDST(upkeep, costi),
	{}};
BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VAR(costi, 1), get_cost_info},
	{"Building", VAR(buildingi, 1), 0, 0, fnscript<buildingi>},
	{"Landscape", VAR(landscapei, 1)},
	{"Player", VAR(playeri, 1)},
	{"Province", VAR(provincei, 1)},
	{"Script", VAR(script, 1), 0, 0, fnscript<script>},
	{"Unit", VAR(uniti, 1), 0, 0, fnscript<uniti>},
};
BSDATAF(varianti)