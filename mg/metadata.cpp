#include "bsreq.h"
#include "condition.h"
#include "item.h"
#include "rang.h"
#include "skill.h"
#include "variant.h"

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSDATAC(itemi, 128)
BSDATAC(rangi, 16)

BSMETA(actioni) = {
	BSREQ(id),
	{}};
BSMETA(itemi) = {
	BSREQ(id),
	BSFLG(tags, tagi),
	{}};
BSMETA(rangi) = {
	BSREQ(id),
	BSREQ(age),
	BSDST(skills, skilli),
	{}};
BSMETA(skilli) = {
	BSREQ(id),
	BSFLG(help, skilli),
	{}};
BSMETA(tagi) = {
	BSREQ(id),
	BSDST(bonus, actioni),
	BSDST(success, actioni),
	{}};

BSDATA(varianti) = {
	{"NoVariant"},
	{"Action", VAR(actioni, 1)},
	{"Item", VAR(itemi, 1)},
	{"Rang", VAR(rangi, 1)},
	{"Skill", VAR(skilli, 1)},
	{"Tag", VAR(tagi, 1)},
};
BSDATAF(varianti)