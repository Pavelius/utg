#include "bsreq.h"
#include "gender.h"
#include "harmable.h"
#include "item.h"
#include "menu.h"
#include "move.h"
#include "result.h"
#include "risk.h"
#include "special.h"
#include "tag.h"
#include "variant.h"
#include "widget.h"

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(tagi) = {
	BSREQ(id),
	{}};
BSMETA(menu) = {
	BSREQ(id),
	BSREQ(elements),
	{}};
BSDATAC(menu, 256)
BSMETA(resulti) = {
	BSREQ(id),
	{}};
BSMETA(movei) = {
	BSREQ(id),
	BSENM(roll, abilityi),
	{}};
BSMETA(harmi) = {
	BSREQ(id),
	{}};
BSMETA(genderi) = {
	BSREQ(id),
	{}};
BSMETA(riski) = {
	BSREQ(id),
	{}};
BSMETA(speciali) = {
	BSREQ(id),
	{}};
BSMETA(itemi) = {
	BSREQ(id), BSREQ(wear),
	BSFLG(tags, tagi),
	BSFLG(moves, movei),
	{}};
BSDATAC(itemi, 256)

BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VAR(abilityi, 1)},
	{"Item", VAR(itemi, 1)},
	{"Menu", VAR(menu, 1)},
	{"Move", VAR(movei, 1)},
	{"Resource", VAR(harmi, 1)},
	{"Result", VAR(resulti, 1)},
	{"Risk", VAR(riski, 1)},
	{"Specail", VAR(speciali, 1)},
	{"Tag", VAR(tagi, 1)},
	{"Widget", VAR(widget, 1)},
};
BSDATAF(varianti)