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
	BSREQ(avatar),
	{}};
BSDATAC(pirate, 4)
BSMETA(speciali) = {
	BSREQ(id),
	{}};

#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr, 1
BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VRSTD(abilityi)},
	{"Action", VRSTD(actioni)},
	{"Class", VRSTD(classi)},
	{"Gender", VRSTD(genderi)},
	{"Group", VRSTD(groupi)},
	{"Location", VRSTD(locationi)},
	{"Pirate", VRSTD(pirate)},
	{"Special", VRSTD(speciali)},
	{"Value", VRSTD(groupvaluei)},
};
assert_enum(varianti, Value)