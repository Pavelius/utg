#include "bsreq.h"
#include "building.h"
#include "player.h"
#include "unit.h"
#include "variant.h"

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)
BSDATAC(buildingi, 128)
BSDATAC(building, 1024)
BSDATAC(landscapei, 32)
BSDATAC(playeri, 16)
BSDATAC(uniti, 48)

BSMETA(buildingi) = {
	BSREQ(id),
	BSDST(effect, costi),
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
	BSDST(attacker, combati),
	BSDST(defender, combati),
	{}};
BSMETA(uniti) = {
	BSREQ(id),
	BSDST(cost, costi),
	BSDST(combat, combati),
	BSDST(upkeep, costi),
	{}};
BSDATA(varianti) = {
	{"NoVariant"},
	{"Building", VAR(buildingi, 1)},
	{"Landscape", VAR(landscapei, 1)},
	{"Unit", VAR(uniti, 1)},
};
BSDATAF(varianti)