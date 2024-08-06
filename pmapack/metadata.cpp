#include "bsreq.h"
#include "crt.h"
#include "image.h"
#include "variant.h"

BSDATAC(imagei, 4096)
BSDATAC(imagea, 256)

BSMETA(imagei) = {
	BSREQ(id),
	BSREQ(name), BSREQ(url), BSREQ(suffix), BSREQ(prefix),
	BSREQ(x), BSREQ(y), BSREQ(sx), BSREQ(sy),
	{}};
BSMETA(imagea) = {
	BSREQ(id),
	{}};

BSDATA(varianti) = {
	{"NoVariant"},
	{"Image", VAR(imagei, 1)},
	{"ImageList", VAR(imagea, 1)},
};
BSDATAF(varianti)
