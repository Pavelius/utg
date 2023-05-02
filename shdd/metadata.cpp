#include "advancement.h"
#include "bsreq.h"
#include "item.h"
#include "list.h"
#include "menu.h"
#include "modifier.h"
#include "race.h"
#include "statable.h"
#include "script.h"
#include "widget.h"

NOBSDATA(dice)
NOBSDATA(itemi::weaponi)
NOBSDATA(variants)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSDATAC(itemi, 256)
BSDATAC(itempoweri, 64)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(dice) = {
	BSREQ(c), BSREQ(d), BSREQ(b), BSREQ(m),
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
BSMETA(modifieri) = {
	BSREQ(id),
	{}};
BSMETA(racei) = {
	BSREQ(id),
	{}};
BSMETA(script) = {
	BSREQ(id),
	{}};
BSMETA(weari) = {
	BSREQ(id),
	{}};
BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VAR(abilityi, 1)},
	{"Advancement", VAR(advancement, 2)},
	{"Ancestry", VAR(racei, 1)},
	{"Item", VAR(itemi, 1), 0, 0, fnscript<itemi>},
	{"ItemPower", VAR(itempoweri, 1)},
	{"Feat", VAR(feati, 1)},
	{"List", VAR(listi, 1)},
	{"Menu", VAR(menu, 1)},
	{"Modifier", VAR(modifieri, 1)},
	{"Script", VAR(script, 1)},
	{"Widget", VAR(widget, 1)},
};
BSDATAF(varianti)