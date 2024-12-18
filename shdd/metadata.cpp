#include "advancement.h"
#include "area.h"
#include "bsreq.h"
#include "creature.h"
#include "item.h"
#include "gender.h"
#include "groupname.h"
#include "location.h"
#include "list.h"
#include "menu.h"
#include "modifier.h"
#include "monster.h"
#include "nametable.h"
#include "questlist.h"
#include "race.h"
#include "spell.h"
#include "special.h"
#include "statable.h"
#include "script.h"
#include "widget.h"

NOBSDATA(dice)
NOBSDATA(item)

BSDATAC(areai, 128)
BSDATAC(creature, 1024)
BSDATAC(itemi, 256)
BSDATAC(locationi, 2048)
BSDATAC(monsteri, 64)
BSDATAC(racei, 32)
BSDATAC(spelli, 256)
BSDATAC(speciali, 128)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(areai) = {
	BSREQ(id),
	{}};
BSMETA(classi) = {
	BSREQ(id),
	{}};
BSMETA(creature) = {	
	BSDST(abilities, abilityi),
	BSFLG(feats, feati),
	BSENM(kind, classi),
	BSFLG(usable, itemi),
	BSFLG(mastery, itemi),
	BSENM(ancestry, racei),
	{}};
BSMETA(dice) = {
	BSREQ(c), BSREQ(d), BSREQ(b), BSREQ(m),
	{}};
BSMETA(feati) = {
	BSREQ(id),
	{}};
BSMETA(itemi) = {
	BSREQ(id),
	BSREQ(cost), BSREQ(damage), BSREQ(count), BSREQ(slot), BSREQ(attack),
	BSFLG(flags, feati),
	BSENM(wear, weari),
	BSREQ(use),
	BSREQ(wearing),
	BSREQ(powers),
	{}};
BSMETA(item) = {
	{}};
BSMETA(locationi) = {
	BSENM(area, areai),
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
BSMETA(speciali) = {
	BSREQ(id),
	BSENM(save, abilityi), BSREQ(dc),
	BSREQ(effect),
	{}};
BSMETA(spelli) = {
	BSREQ(id),
	BSREQ(tier), BSREQ(rounds),
	BSFLG(classes, classi),
	BSREQ(enchant), BSREQ(summon),
	{}};
BSMETA(statei) = {
	BSREQ(id),
	{}};
BSMETA(weari) = {
	BSREQ(id),
	{}};

void creature_getproperty(const void* object, variant v, stringbuilder& sb);
template<> void ftinfo<dice>(const void* object, stringbuilder& sb);

BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VAR(abilityi, 1), 0, 0, fnscript<abilityi>},
	{"Advancement", VAR(advancement, 2)},
	{"Ancestry", VAR(racei, 1)},
	{"Area", VAR(areai, 1)},
	{"Class", VAR(classi, 1), 0, 0, fnscript<classi>},
	{"Consumable", VAR(consumablei, 1), 0, 0, fnscript<consumablei>},
	{"Creature", VAR(creature, 0), ftinfo<creature>, creature_getproperty},
	{"Item", VAR(itemi, 1), 0, 0, fnscript<itemi>},
	{"ItemObject", VAR(item, 0), ftinfo<item>},
	{"Feat", VAR(feati, 1)},
	{"Gender", VAR(genderi, 1), 0, 0, fnscript<genderi>},
	{"GroupName", VAR(groupname, 2), 0, 0, 0, 0, read_group_names},
	{"List", VAR(listi, 1)},
	{"Location", VAR(locationi, 0)},
	{"Menu", VAR(menu, 1)},
	{"Modifier", VAR(modifieri, 1), 0, 0, fnscript<modifieri>},
	{"Monster", VAR(monsteri, 1)},
	{"Quest", VAR(questlist, 1), 0, 0, fnscript<questlist>, 0, questlist_read},
	{"Race", VAR(racei, 1), 0, 0, fnscript<racei>},
	{"Script", VAR(script, 1), 0, 0, fnscript<script>},
	{"Spell", VAR(spelli, 1)},
	{"State", VAR(statei, 1), 0, 0, fnscript<statei>},
	{"RollTable", VAR(nametable, 1), 0, 0, fnscript<nametable>, 0, read_nametable},
	{"Widget", VAR(widget, 1)},
};
BSDATAF(varianti)