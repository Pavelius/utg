#include "action.h"
#include "advance.h"
#include "alignment.h"
#include "bsreq.h"
#include "creature.h"
#include "gender.h"
#include "itemlay.h"
#include "list.h"
#include "menu.h"
#include "modifier.h"
#include "monster.h"
#include "ongoing.h"
#include "organization.h"
#include "race.h"
#include "randomizer.h"
#include "reaction.h"
#include "scenery.h"
#include "script.h"
#include "widget.h"

NOBSDATA(attacki)
NOBSDATA(interval)

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
BSDATAC(scenei, 64)
BSDATAC(scenery, 2048)

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
BSMETA(alignmenti) = {
	BSREQ(id),
	{}};
BSMETA(attacki) = {
	BSREQ(id),
	BSREQ(number),
	BSREQ(damage),
	BSREQ(hit),
	{}};
BSMETA(classi) = {
	BSREQ(id),
	BSREQ(hd),
	BSDST(minimal, abilityi),
	BSENM(prime, abilityi),
	BSREQ(experience),
	BSREQ(magic),
	BSFLG(allow, itemi),
	BSFLG(reputation, organizationi),
	BSREQ(origin),
	{}};
BSMETA(creature) = {
	BSDST(abilities, abilityi),
	{}};
BSMETA(durationi) = {
	BSREQ(id),
	{}};
BSMETA(feati) = {
	BSREQ(id),
	{}};
BSMETA(groupi) = {
	BSREQ(id),
	{}};
BSMETA(interval) = {
	BSREQ(min), BSREQ(max),
	{}};
BSMETA(itemi) = {
	BSREQ(id),
	BSREQ(cost), BSREQ(weight), BSREQ(count),
	BSFLG(flags, feati),
	BSENM(wear, weari),
	BSREQ(number),
	BSREQ(damage),
	BSREQ(ammunition),
	BSREQ(use),
	BSREQ(wearing),
	BSREQ(powers),
	{}};
BSMETA(modifieri) = {
	BSREQ(id),
	{}};
BSMETA(monsteri) = {
	BSREQ(id),
	BSREQ(feats),
	BSREQ(dungeon),
	BSREQ(wilderness),
	BSREQ(dungeon_group), BSREQ(dungeon_lair),
	BSREQ(wilderness_group), BSREQ(wilderness_lair),
	BSREQ(origin),
	BSREQ(treasure),
	BSREQ(magic),
	{}};
BSMETA(organizationi) = {
	BSREQ(id),
	BSFLG(allies, organizationi), BSFLG(enemies, organizationi),
	BSDST(abilities, orgstati),
	{}};
BSMETA(orgstati) = {
	BSREQ(id),
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
BSMETA(reactioni) = {
	BSREQ(id),
	{}};
BSMETA(scenei) = {
	BSREQ(id),
	BSREQ(actions), BSREQ(elements), BSREQ(script),
	BSREQ(encounter_chance),
	BSENM(encounter, groupi),
	{}};
BSMETA(scenery) = {
	BSENM(type, scenei),
	BSREQ(parent),
	{}};
BSMETA(spelli) = {
	BSREQ(id),
	BSREQ(level),
	BSREQ(count), BSREQ(targets),
	BSENM(duration, durationi),
	BSENM(range, rangei),
	BSENM(dispell, spelli),
	BSENM(save_negates, abilityi),
	BSENM(save_halves, abilityi),
	BSENM(reversed, spelli), BSENM(alternate, spelli), BSENM(enchant, spelli),
	{}};
BSMETA(weari) = {
	BSREQ(id),
	{}};
BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VAR(abilityi, 1), 0, 0, fnscript<abilityi>},
	{"Action", VAR(actioni, 1)},
	{"Advance", VAR(advancei, 3)},
	{"Alignment", VAR(alignmenti, 1), 0, 0, fnscript<alignmenti>},
	{"Class", VAR(classi, 1)},
	{"Creature", VAR(creature, 0), creature::getstatus, creature::getproperty},
	{"Feat", VAR(feati, 1), 0, 0, fnscript<feati>},
	{"Gender", VAR(genderi, 1)},
	{"Item", VAR(itemi, 1), 0, 0, fnscript<itemi>},
	{"List", VAR(listi, 1), 0, 0, fnscript<listi>, fntest<listi>},
	{"Menu", VAR(menu, 1)},
	{"Modifier", VAR(modifieri, 1), 0, 0, fnscript<modifieri>},
	{"Monster", VAR(monsteri, 1)},
	{"Organization", VAR(organizationi, 1)},
	{"Race", VAR(racei, 1)},
	{"Random", VAR(randomizeri, 1), 0, 0, fnscript<randomizeri>},
	{"Reaction", VAR(reactioni, 1)},
	{"SceneType", VAR(scenei, 1), 0, 0},
	{"Scene", VAR(scenery, 2), 0, 0},
	{"Script", VAR(script, 1), 0, 0, fnscript<script>, fntest<script>},
	{"Spell", VAR(spelli, 1), 0, 0, fnscript<spelli>, fntest<spelli>},
	{"Widget", VAR(widget, 1)},
};
BSDATAF(varianti)