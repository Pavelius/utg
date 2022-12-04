#include "ability.h"
#include "bsreq.h"
#include "classi.h"
#include "crt.h"
#include "creature.h"
#include "dice.h"
#include "utg.h"

NOBSDATA(dice)
NOBSDATA(variants)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSDATAC(abilityi, 256)
BSDATAC(classi, 16)
BSDATAC(creature, 1024)
BSDATAC(skilli, 32)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(classi) = {
	BSREQ(id),
	BSFLG(skills, skilli),
	BSREQ(hd), BSREQ(force),
	{}};
BSMETA(creature) = {
	{}};
BSMETA(dice) = {
	BSREQ(c), BSREQ(d), BSREQ(b), BSREQ(m),
	{}};
BSMETA(skilli) = {
	BSREQ(id),
	BSENM(ability, abilityi),
	{}};
BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VAR(abilityi, 1)},
	{"Class", VAR(classi, 1)},
	{"Creature", VAR(creature, 0)},
	{"Menu", VAR(menu, 1)},
	{"Skill", VAR(skilli, 1)},
	{"Widget", VAR(widget, 1)},
};
BSDATAF(varianti)