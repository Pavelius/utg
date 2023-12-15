#include "advance.h"
#include "bsreq.h"
#include "item.h"
#include "game.h"
#include "gender.h"
#include "list.h"
#include "menu.h"
#include "modifier.h"
#include "monster.h"
#include "ongoing.h"
#include "rolldice.h"
#include "script.h"
#include "widget.h"

NOBSDATA(dice)
NOBSDATA(itemi::weaponi)

BSDATAC(advancei, 256)
BSDATAC(classi, 16)
BSDATAC(creature, 256)
BSDATAC(durationi, 32)
BSDATAC(itemi, 256)
BSDATAC(itempoweri, 64)
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
BSMETA(advancei) = {
	BSREQ(type), BSREQ(level), BSREQ(id),
	BSREQ(elements),
	{}};
BSMETA(feati) = {
	BSREQ(id),
	{}};
BSMETA(itempoweri) = {
	BSREQ(id),
	BSREQ(magic),
	BSREQ(condition),
	BSREQ(damage),
	BSREQ(wearing),
	{}};
BSMETA(itemi) = {
	BSREQ(id),
	BSREQ(cost), BSREQ(weight), BSREQ(count),
	BSFLG(flags, feati),
	BSENM(wear, weari),
	BSREQ(weapon),
	BSREQ(use),
	BSREQ(wearing),
	BSREQ(powers),
	{}};
BSMETA(itemi::weaponi) = {
	BSREQ(damage),
	BSENM(ammunition, itemi),
	{}};
BSMETA(monsteri) = {
	BSREQ(id),
	BSREQ(feats),
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
	{"Advance", VAR(advancei, 3)},
	{"Class", VAR(classi, 1)},
	{"Creature", VAR(creature, 0), creature::getstatus, creature::getproperty},
	{"Dice", VAR(rolldice, 1)},
	{"Feat", VAR(feati, 1), 0, 0, fnscript<feati>},
	{"Gender", VAR(genderi, 1)},
	{"Item", VAR(itemi, 1), 0, 0, fnscript<itemi>},
	{"ItemPower", VAR(itempoweri, 1)},
	{"List", VAR(listi, 1)},
	{"Menu", VAR(menu, 1)},
	{"Modifier", VAR(monsteri, 1), 0, 0, fnscript<modifieri>},
	{"Monster", VAR(monsteri, 1)},
	{"Spell", VAR(spelli, 1)},
	{"Widget", VAR(widget, 1)},
};
BSDATAF(varianti)