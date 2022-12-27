#include "animal.h"
#include "bsreq.h"
#include "condition.h"
#include "creature.h"
#include "item.h"
#include "enviroment.h"
#include "rang.h"
#include "skill.h"
#include "variant.h"
#include "wise.h"

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSDATAC(animali, 64)
BSDATAC(itemi, 128)
BSDATAC(enviromenti, 32)
BSDATAC(hero, 32)
BSDATAC(rangi, 16)

BSMETA(actioni) = {
	BSREQ(id),
	{}};
BSMETA(animali) = {
	BSREQ(id),
	BSENM(gender, genderi),
	BSREQ(nature),
	{}};
BSMETA(itemi) = {
	BSREQ(id),
	BSFLG(tags, tagi),
	{}};
BSMETA(enviromenti) = {
	BSREQ(id),
	BSENM(type, enviroment_typei),
	{}};
BSMETA(enviroment_typei) = {
	BSREQ(id),
	{}};
BSMETA(rangi) = {
	BSREQ(id),
	BSREQ(age),
	BSREQ(party_count),
	BSREQ(wises),
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
BSMETA(wisei) = {
	BSREQ(id),
	BSREQ(subject),
	{}};

BSDATA(varianti) = {
	{"NoVariant"},
	{"Action", VAR(actioni, 1)},
	{"Animal", VAR(animali, 1)},
	{"Item", VAR(itemi, 1)},
	{"Enviroment", VAR(enviromenti, 1)},
	{"Rang", VAR(rangi, 1)},
	{"Skill", VAR(skilli, 1)},
	{"Tag", VAR(tagi, 1)},
	{"Wise", VAR(wisei, 1)},
};
BSDATAF(varianti)