#include "action.h"
#include "advance.h"
#include "bsreq.h"
#include "creature.h"
#include "gender.h"
#include "itemlay.h"
#include "list.h"
#include "menu.h"
#include "modifier.h"
#include "monster.h"
#include "ongoing.h"
#include "race.h"
#include "randomizer.h"
#include "scenery.h"
#include "script.h"
#include "widget.h"

NOBSDATA(attacki)
NOBSDATA(dice)

BSDATAC(advancei, 256)
BSDATAC(actioni, 256)
BSDATAC(classi, 16)
BSDATAC(creature, 256)
BSDATAC(durationi, 32)
BSDATAC(itemi, 256)
BSDATAC(itemlay, 4096)
BSDATAC(monsteri, 128)
BSDATAC(ongoing, 256)
BSDATAC(racei, 32)
BSDATAC(scenery, 2048)
BSDATAC(sceneryi, 128)
BSDATAC(spelli, 128)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(actioni) = {
	BSREQ(id),
	{}};
BSMETA(advancei) = {
	BSREQ(type), BSREQ(level), BSREQ(id),
	BSREQ(elements),
	{}};
BSMETA(attacki) = {
	BSREQ(count),
	BSREQ(damage),
	{}};
BSMETA(classi) = {
	BSREQ(id),
	BSREQ(hd),
	BSDST(minimal, abilityi),
	BSENM(prime, abilityi),
	BSREQ(experience),
	BSREQ(magic),
	BSFLG(allow, itemi),
	BSREQ(origin),
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
BSMETA(feati) = {
	BSREQ(id),
	{}};
BSMETA(itemi) = {
	BSREQ(id),
	BSREQ(cost), BSREQ(weight), BSREQ(count),
	BSFLG(flags, feati),
	BSENM(wear, weari),
	BSREQ(damage),
	BSREQ(ammunition),
	BSREQ(use),
	BSREQ(wearing),
	BSREQ(powers),
	{}};
BSMETA(monsteri) = {
	BSREQ(id),
	BSREQ(feats),
	BSREQ(dungeon),
	BSREQ(wilderness),
	BSREQ(attacks),
	BSREQ(origin),
	BSREQ(basic),
	BSREQ(treasure),
	{}};
BSMETA(randomizeri) = {
	BSREQ(id),
	BSREQ(chance),
	{}};
BSMETA(racei) = {
	BSREQ(id),
	BSFLG(hate, racei),
	{}};
BSMETA(rangei) = {
	BSREQ(id),
	{}};
BSMETA(sceneryi) = {
	BSREQ(id),
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
	{"Action", VAR(actioni, 1)},
	{"Advance", VAR(advancei, 3)},
	{"Class", VAR(classi, 1)},
	{"Creature", VAR(creature, 0), creature::getstatus, creature::getproperty},
	{"Feat", VAR(feati, 1), 0, 0, fnscript<feati>},
	{"Gender", VAR(genderi, 1)},
	{"Item", VAR(itemi, 1), 0, 0, fnscript<itemi>},
	{"List", VAR(listi, 1)},
	{"Menu", VAR(menu, 1)},
	{"Modifier", VAR(monsteri, 1), 0, 0, fnscript<modifieri>},
	{"Monster", VAR(monsteri, 1)},
	{"Race", VAR(racei, 1)},
	{"Random", VAR(randomizeri, 1), 0, 0, fnscript<randomizeri>},
	{"Script", VAR(script, 1), 0, 0, fnscript<script>, fntest<script>},
	{"Spell", VAR(spelli, 1)},
	{"Widget", VAR(widget, 1)},
};
BSDATAF(varianti)