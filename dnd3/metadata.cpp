#include "ability.h"
#include "background.h"
#include "bsreq.h"
#include "crt.h"
#include "item.h"
#include "list.h"
#include "numberlist.h"
#include "room.h"
#include "script.h"
#include "variant.h"
#include "widget.h"

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};

BSDATAC(backgroundi, 64)
BSDATAC(itemi, 512)
BSDATAC(roomi, 16)
BSDATAD(variant)

NOBSDATA(dice)
NOBSDATA(weaponi)

BSMETA(abilityi) = {
	BSREQ(id),
	BSREQ(formula),
	{}};
BSMETA(backgroundi) = {
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
	{"Ability", VAR(abilityi, 1), 0, 0},
	{"Background", VAR(backgroundi, 1), 0, 0},
	{"Item", VAR(itemi, 1), 0, 0},
	{"List", VAR(listi, 1), 0, 0},
	{"NumberList", VAR(numberlist, 1), 0, 0},
	{"Rate", VAR(magici, 1), 0, 0},
	{"Script", VAR(script, 1), 0, 0},
	{"Widget", VAR(widget, 1)},
};
BSDATAF(varianti)
