#include "alignment.h"
#include "bsreq.h"
#include "class.h"
#include "creature.h"
#include "diety.h"
#include "groupname.h"
#include "list.h"
#include "option.h"
#include "questlist.h"
#include "script.h"
#include "widget.h"

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)
BSDATAC(creature, 4)
BSDATAC(classi, 16)
BSDATAC(dietyi, 32)
BSDATAC(itemi, 256)
BSDATAC(menu, 256)
BSDATAC(optioni, 256)

BSMETA(creature) = {
	BSENM(type, classi),
	BSENM(race, racei),
	BSENM(alignment, alignmenti),
	BSDST(abilities, abilityi),
	BSENM(diety, dietyi),
	{}};
BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(alignmenti) = {
	BSREQ(id),
	{}};
BSMETA(classi) = {
	BSREQ(id),
	BSENM(abilities, abilityi),
	BSREQ(damage),
	{}};
BSMETA(optioni) = {
	BSREQ(id),
	{}};
BSMETA(tagi) = {
	BSREQ(id),
	{}};
BSMETA(dietyi) = {
	BSREQ(id),
	{}};
BSMETA(menu) = {
	BSREQ(id), BSREQ(source),
	BSREQ(elements),
	{}};
BSMETA(movei) = {
	BSREQ(id),
	{}};
BSMETA(genderi) = {
	BSREQ(id),
	{}};
BSMETA(racei) = {
	BSREQ(id),
	{}};
BSMETA(script) = {
	BSREQ(id),
	{}};
BSMETA(itemi) = {
	BSREQ(id), BSREQ(weight), BSREQ(coins), BSREQ(uses), BSREQ(forward),
	BSREQ(damage), BSREQ(armor), BSREQ(pierce), BSREQ(heal), 
	BSFLG(tags, tagi),
	BSFLG(moves, movei),
	{}};
BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VAR(abilityi, 1)},
	{"Advancement", VAR(advancement, 2)},
	{"Alignment", VAR(alignmenti, 1)},
	{"Class", VAR(classi, 1)},
	{"Creature", VAR(creature, 0), ftinfo<creature>, creature::getpropertyst},
	{"Diety", VAR(dietyi, 1)},
	{"Front", VAR(front, 1)},
	{"Gender", VAR(genderi, 1)},
	{"GroupName", VAR(groupname, 2), 0, 0, 0, 0, read_group_names},
	{"Item", VAR(itemi, 1), 0, 0, fnscript<itemi>, fntest<itemi>},
	{"List", VAR(listi, 1)},
	{"Menu", VAR(menu, 1)},
	{"Move", VAR(movei, 1)},
	{"Option", VAR(optioni, 1)},
	{"Quest", VAR(questlist, 1), 0, 0, 0, 0, questlist_read},
	{"Race", VAR(racei, 1)},
	{"Script", VAR(script, 1)},
	{"Tag", VAR(tagi, 1)},
	{"Widget", VAR(widget, 1)},
};
BSDATAF(varianti)