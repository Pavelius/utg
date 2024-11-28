#include "ability.h"
#include "advance.h"
#include "bsreq.h"
#include "chooseable.h"
#include "creature.h"
#include "enumerator.h"
#include "item.h"
#include "list.h"
#include "material.h"
#include "modifier.h"
#include "monster.h"
#include "numberlist.h"
#include "panel.h"
#include "roll.h"
#include "room.h"
#include "script.h"
#include "variant.h"
#include "widget.h"

BSDATAC(advancementi, 2048)
BSDATAC(consumablei, 8)
BSDATAC(creature, 512)
BSDATAC(itemi, 512)
BSDATAC(monsteri, 256)
BSDATAC(materiali, 32)
BSDATAC(paneli, 32)
BSDATAC(roomi, 16)
BSDATAC(rolli, 64)

NOBSDATA(weaponi)

BSMETA(abilityi) = {
	BSREQ(id),
	BSREQ(formula),
	{}};
BSMETA(advancementi) = {
	BSREQ(type), BSREQ(id),
	BSREQ(elements),
	{}};
BSMETA(enumi) = {
	BSREQ(id), BSREQ(type),
	{}};
BSMETA(consumablei) = {
	BSREQ(id),
	BSREQ(maximum),
	{}};
BSMETA(formulai) = {
	BSREQ(id),
	BSREQ(formula),
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
BSMETA(paneli) = {
	BSREQ(id),
	BSREQ(width), BSREQ(height), BSREQ(column),
	BSREQ(elements),
	{}};
BSMETA(rangei) = {
	BSREQ(id),
	{}};
BSMETA(rolli) = {
	BSREQ(id),
	BSREQ(c), BSREQ(d), BSREQ(p), BSREQ(w), BSREQ(b), BSREQ(m),
	{}};
BSMETA(roomi) = {
	BSREQ(avatar),
	{}};
BSMETA(weaponi) = {
	BSREQ(bonus), BSREQ(ac), BSREQ(magic),
	BSENM(damage, rolli),
	BSENM(range, rangei),
	{}};

BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VAR(abilityi, 1), 0, 0, fnscript<abilityi>},
	{"Advancement", VAR(advancementi, 2)},
	{"Consumable", VAR(consumablei, 1), 0, 0, fnscript<consumablei>},
	{"DiceRoll", VAR(rolli, 1), 0, 0, fnscript<rolli>},
	{"Element", VAR(enumi, 1), 0, 0},
	{"Formula", VAR(formulai, 1), 0, 0, fnscript<formulai>},
	{"Group", VAR(enumgroupi, 1), 0, 0},
	{"Item", VAR(itemi, 1), 0, 0},
	{"List", VAR(listi, 1), 0, 0, fnscript<listi>},
	{"Material", VAR(materiali, 1), 0, 0, fnscript<materiali>},
	{"Modifier", VAR(modifieri, 1), 0, 0, fnscript<modifieri>},
	{"Monster", VAR(monsteri, 1), 0, 0, fnscript<monsteri>},
	{"NumberList", VAR(numberlist, 1), 0, 0},
	{"Panel", VAR(paneli, 1), 0, 0},
	{"Rate", VAR(magici, 1), 0, 0},
	{"Script", VAR(script, 1), 0, 0, fnscript<script>},
	{"Widget", VAR(widget, 1)},
};
BSDATAF(varianti)
