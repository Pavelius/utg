#include "bsreq.h"
#include "main.h"

NOBSDATA(variants)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSDATAC(planeti, 512)
BSDATAC(systemi, 32)

BSMETA(tagi) = {
	BSREQ(id),
	{}};
BSMETA(planeti) = {
	BSREQ(id),
	BSREQ(system),
	{}};
BSMETA(systemi) = {
	BSREQ(id),
	{}};

#define VAR(T, KN) bsmeta<T>::meta, bsdata<T>::source_ptr, KN
BSDATA(varianti) = {
	{"NoVariant"},
	{"Planet", VAR(planeti, 1)},
	{"System", VAR(systemi, 1)},
	{"Tag", VAR(tagi, 1)},
};
BSDATAF(varianti)