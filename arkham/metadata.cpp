#include "bsreq.h"
#include "gender.h"
#include "main.h"
#include "menu.h"
#include "variant.h"

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)
BSDATAC(cardi, 256)
BSDATAC(monsteri, 128)
BSDATAC(locationi, 128)
NOBSDATA(point)

#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr

BSMETA(point) = {
	BSREQ(x), BSREQ(y),
	{}};
BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(realmi) = {
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
BSMETA(locationi) = {
	BSREQ(id),
	BSREQ(position),
	BSREQ(neightboard),
	{}};
BSMETA(monsteri) = {
	BSREQ(id),
	BSENM(realm, realmi),
	BSREQ(count),
	BSREQ(toughness), BSREQ(alertness),
	BSREQ(horror_modifier), BSREQ(horror_damage),
	BSREQ(combat_modifier), BSREQ(combat_damage),
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
	{"Location", VRSTD(locationi), 1},
	{"Menu", VRSTD(menu), 1},
	{"Monster", VRSTD(monsteri), 1},
	{"Realm", VRSTD(realmi), 1},
	{"Tag", VRSTD(tagi), 1},
};
BSDATAF(varianti)