#include "bsreq.h"
#include "crt.h"
#include "image.h"
#include "variant.h"

BSDATAC(imagei, 4096)
BSDATAC(imagea, 256)
NOBSDATA(point)

BSMETA(imagei) = {
	BSREQ(id),
	BSREQ(name), BSREQ(url), BSREQ(suffix), BSREQ(prefix),
	BSREQ(position), BSREQ(offset),
	{}};
BSMETA(imagea) = {
	BSREQ(id),
	{}};
BSMETA(point) = {
	BSREQ(x), BSREQ(y),
	{}};

BSDATA(varianti) = {
	{"NoVariant"},
	{"Image", VAR(imagei, 1)},
	{"ImageList", VAR(imagea, 1)},
};
BSDATAF(varianti)
