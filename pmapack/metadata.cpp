#include "bsreq.h"
#include "image.h"
#include "variant.h"

BSDATAC(imagei, 4096)
BSDATAC(imagea, 256)
NOBSDATA(point)

BSMETA(imagei) = {
	BSREQ(id),
	BSREQ(name), BSREQ(url), BSREQ(dest_url), BSREQ(ext),
	BSREQ(position), BSREQ(center), BSREQ(size),
	BSREQ(param), BSREQ(bpp),
	{}};
BSMETA(imagea) = {
	BSREQ(id),
	BSREQ(name), BSREQ(url), BSREQ(dest_url), BSREQ(ext),
	BSREQ(position), BSREQ(center), BSREQ(size),
	BSREQ(param), BSREQ(bpp),
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
