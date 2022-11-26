#include "bsreq.h"
#include "variant.h"
#include "widget.h"

NOBSDATA(variants)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSDATA(varianti) = {
	{"NoVariant"},
	{"Widget", VAR(widget, 1)},
};
BSDATAF(varianti)