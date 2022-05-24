#include "bsreq.h"
#include "main.h"

NOBSDATA(variants)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSMETA(tagi) = {
	BSREQ(id),
	{}};

#define VAR(T, KN) bsmeta<T>::meta, bsdata<T>::source_ptr, KN
BSDATA(varianti) = {
	{"NoVariant"},
	{"Tag", VAR(tagi, 1)},
};
BSDATAF(varianti)