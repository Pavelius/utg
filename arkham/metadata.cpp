#include "bsreq.h"
#include "gender.h"
#include "main.h"
#include "menu.h"
#include "variant.h"

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)
BSDATAC(cardi, 256)

#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr
BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(cardtypei) = {
	BSREQ(id),
	{}};
BSMETA(genderi) = {
	BSREQ(id),
	{}};
BSMETA(tagi) = {
	BSREQ(id),
	{}};
BSMETA(cardi) = {
	BSREQ(id),
	BSENM(type, cardtypei),
	BSREQ(hands), BSREQ(cost), BSREQ(count), BSREQ(difficult), BSREQ(bonus), BSREQ(pay),
	BSREQ(effect),
	BSREQ(abilities),
	BSFLG(tags, tagi),
	{}};

BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VRSTD(abilityi), 1},
	{"Card", VRSTD(cardi), 1},
	{"CardType", VRSTD(cardtypei), 1},
	{"Tag", VRSTD(tagi), 1},
	{"Menu", VRSTD(menu), 1},
};
BSDATAF(varianti)