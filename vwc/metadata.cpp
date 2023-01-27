#include "bsreq.h"
#include "code_package.h"
#include "variant.h"
#include "widget.h"

using namespace code;

NOBSDATA(variants)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSDATAC(package, 512)

BSDATA(varianti) = {
	{"NoVariant"},
	{"Widget", VAR(widget, 1)},
};
BSDATAF(varianti)