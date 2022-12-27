#include "animal.h"
#include "bsreq.h"
#include "condition.h"
#include "enviroment.h"
#include "item.h"
#include "list.h"
#include "hero.h"
#include "questlist.h"
#include "rang.h"
#include "script.h"
#include "skill.h"
#include "trait.h"
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
	BSREQ(skills), BSREQ(traits),
	{}};
BSMETA(enviroment_typei) = {
	BSREQ(id),
	{}};
BSMETA(rangi) = {
	BSREQ(id),
	BSREQ(age),
	BSREQ(party_count),
	BSREQ(wises), BSREQ(talented), BSREQ(convice), BSREQ(mentors), BSREQ(specialization),
	BSDST(skills, skilli),
	{}};
BSMETA(script) = {
	BSREQ(id),
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
BSMETA(traiti) = {
	BSREQ(id),
	BSFLG(advantages, skilli),
	BSFLG(disadvantages, skilli),
	{}};
BSMETA(wisei) = {
	BSREQ(id),
	BSREQ(subject),
	{}};

BSDATA(varianti) = {
	{"NoVariant"},
	{"Action", VAR(actioni, 1)},
	{"Animal", VAR(animali, 1)},
	{"Enviroment", VAR(enviromenti, 1)},
	{"Item", VAR(itemi, 1)},
	{"List", VAR(listi, 1)},
	{"Rang", VAR(rangi, 1)},
	{"Script", VAR(script, 1), 0, 0, fnscript<script>},
	{"Skill", VAR(skilli, 1), 0, 0, fnscript<skilli>},
	{"Tag", VAR(tagi, 1)},
	{"Trait", VAR(traiti, 1), 0, 0, fnscript<traiti>},
	{"Wise", VAR(wisei, 1)},
};
BSDATAF(varianti)