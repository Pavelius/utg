#include "bsreq.h"
#include "gender.h"
#include "main.h"
#include "menu.h"
#include "variant.h"

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)
BSDATAC(cardprotoi, 256)
BSDATAC(investigator, 32)
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
BSMETA(speciali) = {
	BSREQ(id),
	{}};
BSMETA(cardtypei) = {
	BSREQ(id),
	{}};
BSMETA(genderi) = {
	BSREQ(id),
	{}};
BSMETA(scripti) = {
	BSREQ(id),
	{}};
BSMETA(gamefi) = {
	BSREQ(id),
	{}};
BSMETA(tagi) = {
	BSREQ(id),
	{}};
BSMETA(locationi) = {
	BSREQ(id),
	BSENM(type, cardtypei),
	BSREQ(position),
	BSREQ(neightboard),
	{}};
BSMETA(cardprotoi) = {
	BSREQ(id),
	BSENM(type, cardtypei),
	BSDST(abilities, abilityi),
	BSREQ(hands), BSREQ(cost), BSREQ(count), BSREQ(difficult), BSREQ(bonus), BSREQ(pay),
	BSENM(special, speciali),
	BSREQ(effect),
	BSFLG(tags, tagi),
	BSFLG(realms, realmi),
	{}};
BSMETA(investigator) = {
	BSREQ(id),
	BSDST(abilities, abilityi),
	BSFLG(rerollall, abilityi),
	BSFLG(tought, abilityi),
	BSFLG(restore, abilityi),
	BSFLG(pickextra, cardtypei),
	BSFLG(scavenge, cardtypei),
	BSREQ(extra),
	BSREQ(location),
	{}};

BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VRSTD(abilityi), 1},
	{"Card", VRSTD(cardprotoi), 1},
	{"CardType", VRSTD(cardtypei), 1},
	{"GameFlag", VRSTD(gamefi), 1},
	{"Investigator", VRSTD(investigator), 1},
	{"Location", VRSTD(locationi), 1},
	{"Menu", VRSTD(menu), 1},
	{"Realm", VRSTD(realmi), 1},
	{"Script", VRSTD(scripti), 1},
	{"Tag", VRSTD(tagi), 1},
};
BSDATAF(varianti)