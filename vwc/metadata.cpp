#include "bsreq.h"
#include "variant.h"
#include "viewpackage.h"
#include "widget.h"

using namespace code;

NOBSDATA(variants)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSDATAC(viewpackage, 512)

BSDATA(varianti) = {
	{"NoVariant"},
	{"Widget", VAR(widget, 1)},
};
BSDATAF(varianti)