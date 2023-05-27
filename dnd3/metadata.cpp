#include "ability.h"
#include "background.h"
#include "bsreq.h"
#include "creature.h"
#include "crt.h"
#include "enumerator.h"
#include "group.h"
#include "item.h"
#include "list.h"
#include "material.h"
#include "modifier.h"
#include "monster.h"
#include "numberlist.h"
#include "room.h"
#include "script.h"
#include "variant.h"
#include "widget.h"

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};

BSDATAC(backgroundi, 64)
BSDATAC(creature, 512)
BSDATAC(enumi, 2048)
BSDATAC(itemi, 512)
BSDATAC(monsteri, 256)
BSDATAC(groupi, 256)
BSDATAC(materiali, 32)
BSDATAC(roomi, 16)
BSDATAD(variant)

NOBSDATA(dice)
NOBSDATA(weaponi)

BSMETA(abilityi) = {
	BSREQ(id),
	BSREQ(creating), BSREQ(formula), BSREQ(round),
	{}};
BSMETA(backgroundi) = {
	BSREQ(id),
	{}};
BSMETA(consumablei) = {
	BSREQ(id),
	BSREQ(maximum),
	{}};
BSMETA(groupi) = {
	BSREQ(id),
	{}};
BSMETA(dice) = {
	BSREQ(c), BSREQ(d), BSREQ(b), BSREQ(m),
	{}};
BSMETA(itemi) = {
	BSREQ(id), BSREQ(id_unidentified),
	BSENM(rate, magici),
	BSREQ(weapon),
	BSREQ(count), BSREQ(weight), BSREQ(cost),
	BSREQ(dress), BSREQ(use),
	{}};
BSMETA(magici) = {
	BSREQ(id),
	{}};
BSMETA(materiali) = {
	BSREQ(id),
	{}};
BSMETA(modifieri) = {
	BSREQ(id),
	{}};
BSMETA(monsteri) = {
	BSREQ(id),
	BSREQ(level),
	BSREQ(abilities),
	{}};
BSMETA(numberlist) = {
	BSREQ(id),
	{}};
BSMETA(rangei) = {
	BSREQ(id),
	{}};
BSMETA(roomi) = {
	BSREQ(avatar),
	{}};
BSMETA(script) = {
	BSREQ(id),
	{}};
BSMETA(weaponi) = {
	BSREQ(bonus), BSREQ(ac), BSREQ(magic),
	BSREQ(damage),
	BSENM(range, rangei),
	{}};

BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VAR(abilityi, 1), 0, 0, fnscript<abilityi>},
	{"Background", VAR(backgroundi, 1), 0, 0},
	{"Consumable", VAR(consumablei, 1), 0, 0, fnscript<consumablei>},
	{"Group", VAR(groupi, 1), 0, 0},
	{"Item", VAR(itemi, 1), 0, 0},
	{"List", VAR(listi, 1), 0, 0, fnscript<listi>},
	{"Material", VAR(materiali, 1), 0, 0, fnscript<materiali>},
	{"Modifier", VAR(modifieri, 1), 0, 0, fnscript<modifieri>},
	{"Monster", VAR(monsteri, 1), 0, 0},
	{"NumberList", VAR(numberlist, 1), 0, 0},
	{"Rate", VAR(magici, 1), 0, 0},
	{"Script", VAR(script, 1), 0, 0, fnscript<script>},
	{"Widget", VAR(widget, 1)},
};
BSDATAF(varianti)
