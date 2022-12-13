#include "bsreq.h"
#include "gender.h"
#include "harmable.h"
#include "item.h"
#include "menu.h"
#include "modifier.h"
#include "move.h"
#include "npc.h"
#include "result.h"
#include "risk.h"
#include "script.h"
#include "tag.h"
#include "variant.h"
#include "vagabond.h"
#include "widget.h"

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};

BSDATAC(itemi, 256)
BSDATAC(menu, 256)
BSDATAC(moveoptioni, 256)
BSDATAC(npc, 64)
BSDATAC(vagabond, 16)
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
BSMETA(modifieri) = {
	BSREQ(id),
	{}};
BSMETA(resulti) = {
	BSREQ(id),
	{}};
BSMETA(script) = {
	BSREQ(id),
	{}};
BSMETA(movei) = {
	BSREQ(id),
	BSENM(roll, abilityi),
	{}};
BSMETA(npc) = {
	BSREQ(id),
	BSENM(group, groupi),
	BSDST(harm, harmi),
	BSDSN(inflict, inflict.harm, harmi),
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
BSMETA(itemi) = {
	BSREQ(id), BSREQ(wear),
	BSFLG(tags, tagi),
	BSFLG(moves, movei),
	{}};

BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VAR(abilityi, 1), 0, 0, 0, fnscript<abilityi>},
	{"Item", VAR(itemi, 1)},
	{"Denizen", VAR(npc, 1)},
	{"Menu", VAR(menu, 1)},
	{"Modifier", VAR(modifieri, 1), 0, 0, 0, fnscript<modifieri>},
	{"Move", VAR(movei, 1)},
	{"Resource", VAR(harmi, 1), 0, 0, 0, fnscript<harmi>},
	{"Result", VAR(resulti, 1)},
	{"Risk", VAR(riski, 1)},
	{"Script", VAR(script, 1), 0, 0, 0, fnscript<script>},
	{"Tag", VAR(tagi, 1)},
	{"Widget", VAR(widget, 1)},
};
BSDATAF(varianti)