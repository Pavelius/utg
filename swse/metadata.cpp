#include "ability.h"
#include "advance.h"
#include "bsreq.h"
#include "classi.h"
#include "crt.h"
#include "creature.h"
#include "dice.h"
#include "item.h"
#include "modifier.h"
#include "size.h"
#include "wear.h"
#include "utg.h"

NOBSDATA(dice)
NOBSDATA(variants)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSDATAC(abilityi, 256)
BSDATAC(advancei, 256)
BSDATAC(classi, 16)
BSDATAC(creature, 1024)
BSDATAC(itemi, 256)
BSDATAC(skilli, 32)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(advancei) = {
	BSREQ(level),
	BSREQ(object),
	BSREQ(elements),
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
BSMETA(itemi) = {
	BSREQ(id),
	{}};
BSMETA(modifieri) = {
	BSREQ(id),
	{}};
BSMETA(sizei) = {
	BSREQ(id),
	{}};
BSMETA(skilli) = {
	BSREQ(id),
	BSENM(ability, abilityi),
	{}};
BSMETA(weari) = {
	BSREQ(id),
	{}};
BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VAR(abilityi, 1)},
	{"Advance", VAR(advancei, 2)},
	{"Class", VAR(classi, 1)},
	{"Creature", VAR(creature, 0)},
	{"Item", VAR(itemi, 1)},
	{"Menu", VAR(menu, 1)},
	{"Modifier", VAR(modifieri, 1)},
	{"Size", VAR(sizei, 1)},
	{"Skill", VAR(skilli, 1)},
	{"Widget", VAR(widget, 1)},
};
BSDATAF(varianti)