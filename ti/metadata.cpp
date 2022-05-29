#include "bsreq.h"
#include "main.h"
#include "variant.h"

#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr

BSDATAC(troop, 1024)
BSDATAC(strategyi, 8)
BSDATAC(systemi, 64)
BSDATAC(planeti, 64)
BSDATAC(playeri, 32)
BSDATAC(techi, 128)
BSDATAC(uniti, 32)
BSDATAD(variant)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(colori) = {
	BSREQ(id),
	{}};
BSMETA(planeti) = {
	BSREQ(id),
	BSREQ(resources), BSREQ(influence),
	{}};
BSMETA(playeri) = {
	BSREQ(id),
	BSREQ(commodities),
	{}};
BSMETA(scripti) = {
	BSREQ(id),
	{}};
BSMETA(strategyi) = {
	BSREQ(id),
	BSREQ(initiative),
	BSREQ(primary), BSREQ(secondary),
	{}};
BSMETA(tagi) = {
	BSREQ(id),
	{}};
BSMETA(techi) = {
	BSREQ(id),
	BSENM(color, colori),
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
	{"Planet", VRSTD(planeti), 1},
	{"Player", VRSTD(playeri), 1},
	{"Script", VRSTD(scripti), 1},
	{"Strategy", VRSTD(strategyi), 1},
	{"Unit", VRSTD(uniti), 1},
	{"UnitType", VRSTD(unit_typei), 1},
};
BSDATAF(varianti)