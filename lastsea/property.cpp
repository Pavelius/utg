#include "bsreq.h"
#include "main.h"

#define LNK(V, T) template<> struct bsmeta<V> : bsmeta<T> {};\
template<> struct bsdata<V> : bsdata<T> {};

LNK(ability_s, abilityi)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(actioni) = {
	BSREQ(id),
	BSREQ(result), BSREQ(script),
	BSREQ(outcome1), BSREQ(outcome2), BSREQ(outcome3), BSREQ(outcome4), BSREQ(outcome5), BSREQ(outcome6),
	{}};
BSDATAC(actioni, 128)
BSMETA(genderi) = {
	BSREQ(id),
	{}};
BSMETA(classi) = {
	BSREQ(id),
	BSREQ(types), BSREQ(levelup), BSREQ(maximum),
	BSREQ(exploration), BSREQ(brawl), BSREQ(hunting), BSREQ(aim), BSREQ(swagger), BSREQ(navigation),
	{}};
BSDATAC(classi, 64)
BSMETA(locationi) = {
	BSREQ(id),
	BSREQ(actions),
	{}};
BSDATAC(locationi, 64)
BSMETA(menu) = {
	BSREQ(id), BSREQ(source),
	BSREQ(elements),
	{}};
BSDATAC(menu, 256)
BSMETA(pirate) = {
	{}};
BSDATAC(pirate, 4)
BSMETA(speciali) = {
	BSREQ(id),
	{}};
BSMETA(tagi) = {
	BSREQ(id),
	{}};
BSMETA(widget) = {
	BSREQ(id),
	{}};
BSMETA(shipstati) = {
	BSREQ(id),
	{}};
BSMETA(casei) = {
	BSREQ(type), BSREQ(name), BSREQ(outcome),
	{}};
BSDATAC(casei, 512)
BSMETA(treasurei) = {
	BSREQ(id),
	BSREQ(abilities),
	BSFLG(tags, tagi),
	{}};
BSDATAC(treasurei, 256)

#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr, 1
#define VRREC(T) bsmeta<T>::meta, bsdata<T>::source_ptr, 0
BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VRSTD(abilityi)},
	{"Action", VRSTD(actioni)},
	{"Case", VRREC(casei), FG(varianti::NotFoundByName)},
	{"Card", VRSTD(treasurei)},
	{"Class", VRSTD(classi)},
	{"Gender", VRSTD(genderi)},
	{"Group", VRSTD(groupi)},
	{"Location", VRSTD(locationi)},
	{"Menu", VRSTD(menu)},
	{"Pirate", VRSTD(pirate), 0, 0, 0, pirate::getpropertyst},
	{"Ship", VRSTD(shipstati)},
	{"Special", VRSTD(speciali)},
	{"Tag", VRSTD(tagi)},
	{"Value", VRSTD(groupvaluei)},
	{"Widget", VRSTD(widget)},
};
assert_enum(varianti, Widget)