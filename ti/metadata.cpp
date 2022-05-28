#include "bsreq.h"
#include "main.h"
#include "variant.h"

#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr

BSDATAC(troop, 1024)
BSDATAC(planeti, 64)
BSDATAC(techi, 128)
BSDATAC(uniti, 32)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(tagi) = {
	BSREQ(id),
	{}};
BSMETA(techi) = {
	BSREQ(id),
	BSENM(color, colori),
	{}};
BSMETA(colori) = {
	BSREQ(id),
	{}};
BSMETA(uniti) = {
	BSREQ(id),
	BSENM(type, unit_typei),
	BSDST(abilities, abilityi),
	BSFLG(tags, tagi),
	{}};
BSMETA(unit_typei) = {
	BSREQ(id),
	{}};

BSDATA(varianti) = {
	{"Unit", VRSTD(uniti), 1},
	{"UnitType", VRSTD(unit_typei), 1},
};
BSDATAF(varianti)