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
	BSREQ(actions), BSREQ(image), BSREQ(next),
	{}};
BSDATAC(locationi, 64)
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
BSMETA(casei) = {
	BSREQ(id), BSREQ(type),
	BSREQ(outcome),
	{}};
BSDATAC(casei, 512)
BSMETA(treasurei) = {
	BSREQ(id),
	BSREQ(abilities),
	BSFLG(tags, tagi),
	{}};
BSDATAC(treasurei, 256)

#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr
BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VRSTD(abilityi), 1},
	{"Action", VRSTD(actioni), 1, 0, 0, actioni::sfgetinfo},
	{"Case", VRSTD(casei), 2, 0, 0, casei::sfgetinfo},
	{"Card", VRSTD(treasurei), 1},
	{"Class", VRSTD(classi), 1},
	{"Gender", VRSTD(genderi), 1},
	{"Group", VRSTD(groupi), 1},
	{"Location", VRSTD(locationi), 1},
	{"Menu", VRSTD(menu), 1},
	{"Special", VRSTD(speciali), 1},
	{"Tag", VRSTD(tagi), 1},
	{"Value", VRSTD(groupvaluei), 1},
	{"Widget", VRSTD(widget), 1},
};
assert_enum(varianti, Widget)