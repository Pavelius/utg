#include "bsreq.h"
#include "variant.h"
#include "widget.h"

NOBSDATA(variants)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

#define VAR(T, KN) bsmeta<T>::meta, bsdata<T>::source_ptr, KN
BSDATA(varianti) = {
	{"NoVariant"},
	{"Widget", VAR(widget, 1)},
};
BSDATAF(varianti)