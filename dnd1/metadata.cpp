#include "bsreq.h"
#include "item.h"
#include "game.h"
#include "list.h"
#include "menu.h"
#include "monster.h"
#include "ongoing.h"
#include "rolldice.h"
#include "script.h"
#include "widget.h"

NOBSDATA(dice)
NOBSDATA(itemi::weaponi)
NOBSDATA(variants)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSDATAC(classi, 16)
BSDATAC(creature, 256)
BSDATAC(durationi, 32)
BSDATAC(equipmenti, 256)
BSDATAC(itemi, 256)
BSDATAC(monsteri, 128)
BSDATAC(ongoing, 256)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(classi) = {
	BSREQ(id),
	{}};
BSMETA(creature) = {
	BSDST(abilities, abilityi),
	{}};
BSMETA(dice) = {
	BSREQ(c), BSREQ(d), BSREQ(b), BSREQ(m),
	{}};
BSMETA(durationi) = {
	BSREQ(id),
	{}};
BSMETA(equipmenti) = {
	BSENM(type, classi),
	BSENM(equipment, itemi),
	{}};
BSMETA(feati) = {
	BSREQ(id),
	{}};
BSMETA(itemi) = {
	BSREQ(id),
	BSREQ(cost), BSREQ(weight), BSREQ(count),
	BSFLG(flags, feati),
	BSENM(wear, weari),
	BSREQ(weapon),
	BSREQ(use),
	BSREQ(effect),
	{}};
BSMETA(itemi::weaponi) = {
	BSREQ(damage),
	BSENM(ammunition, itemi),
	{}};
BSMETA(monsteri) = {
	BSREQ(id),
	BSDST(abilities, abilityi),
	BSFLG(feats, feati),
	BSREQ(dungeon),
	BSREQ(wilderness),
	{}};
BSMETA(rangei) = {
	BSREQ(id),
	{}};
BSMETA(rolldice) = {
	BSREQ(id),
	BSREQ(value),
	{}};
BSMETA(spelli) = {
	BSREQ(id),
	BSREQ(level),
	BSREQ(effect),
	BSENM(duration, durationi),
	BSENM(range, rangei),
	{}};
BSMETA(weari) = {
	BSREQ(id),
	{}};
BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VAR(abilityi, 1), 0, 0, fnscript<abilityi>},
	{"Class", VAR(classi, 1)},
	{"Creature", VAR(creature, 0), creature::getstatus, creature::getproperty},
	{"Dice", VAR(rolldice, 1)},
	{"Equipment", VAR(equipmenti, 2)},
	{"Gender", VAR(genderi, 1)},
	{"Item", VAR(itemi, 1)},
	{"List", VAR(listi, 1)},
	{"Menu", VAR(menu, 1)},
	{"Monster", VAR(monsteri, 1)},
	{"Spell", VAR(spelli, 1)},
	{"Widget", VAR(widget, 1)},
};
BSDATAF(varianti)