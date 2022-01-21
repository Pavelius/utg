#include "bsreq.h"
#include "front.h"

BSMETA(front) = {
	BSREQ(id),
	BSREQ(present),
	BSREQ(current), BSREQ(maximum),
	{}};
BSDATAC(front, 256)
