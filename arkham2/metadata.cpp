#include "bsreq.h"
#include "card.h"
#include "gender.h"
#include "script.h"
#include "tag.h"
#include "variant.h"

BSDATAC(cardi, 1024)

BSMETA(cardi) = {
	BSREQ(id),
	BSENM(type, cardti),
	{}};
BSMETA(cardti) = {
	BSREQ(id),
	{}};
BSMETA(tagi) = {
	BSREQ(id),
	{}};

BSDATA(varianti) = {
	{"NoVariant"},
	{"Card", VAR(cardi, 1)},
	{"Script", VAR(script, 1)},
	{"Tag", VAR(tagi, 1)},
};
BSDATAF(varianti)