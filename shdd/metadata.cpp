#include "advancement.h"
#include "bsreq.h"
#include "creature.h"
#include "item.h"
#include "list.h"
#include "menu.h"
#include "modifier.h"
#include "monster.h"
#include "race.h"
#include "spell.h"
#include "statable.h"
#include "script.h"
#include "widget.h"

NOBSDATA(dice)
NOBSDATA(variants)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSDATAC(creature, 1024)
BSDATAC(itemi, 256)
BSDATAC(monsteri, 64)
BSDATAC(spelli, 256)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(classi) = {
	BSREQ(id),
	{}};
BSMETA(dice) = {
	BSREQ(c), BSREQ(d), BSREQ(b), BSREQ(m),
	{}};
BSMETA(feati) = {
	BSREQ(id),
	{}};
BSMETA(itemi) = {
	BSREQ(id),
	BSREQ(cost), BSREQ(damage), BSREQ(count), BSREQ(slot),
	BSFLG(flags, feati),
	BSENM(wear, weari),
	BSREQ(use),
	BSREQ(wearing),
	BSREQ(powers),
	{}};
BSMETA(modifieri) = {
	BSREQ(id),
	{}};
BSMETA(monsteri) = {
	BSREQ(id),
	BSENM(kind, classi),
	BSENM(ancestry, racei),
	BSREQ(feats),
	BSDST(abilities, abilityi),
	{}};
BSMETA(racei) = {
	BSREQ(id),
	{}};
BSMETA(script) = {
	BSREQ(id),
	{}};
BSMETA(spelli) = {
	BSREQ(id),
	BSREQ(tier), BSREQ(rounds),
	BSFLG(classes, classi),
	BSREQ(enchant), BSREQ(summon),
	{}};
BSMETA(weari) = {
	BSREQ(id),
	{}};
BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VAR(abilityi, 1), 0, 0, fnscript<abilityi>},
	{"Advancement", VAR(advancement, 2)},
	{"Ancestry", VAR(racei, 1)},
	{"Item", VAR(itemi, 1), 0, 0, fnscript<itemi>},
	{"Feat", VAR(feati, 1)},
	{"List", VAR(listi, 1)},
	{"Menu", VAR(menu, 1)},
	{"Modifier", VAR(modifieri, 1)},
	{"Monster", VAR(monsteri, 1)},
	{"Script", VAR(script, 1), 0, 0, fnscript<script>},
	{"Spell", VAR(spelli, 1)},
	{"Widget", VAR(widget, 1)},
};
BSDATAF(varianti)