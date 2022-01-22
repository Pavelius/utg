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
BSMETA(groupi) = {
	BSREQ(id),
	{}};
BSDATAC(groupi, 256)
BSMETA(genderi) = {
	BSREQ(id),
	{}};
BSMETA(classi) = {
	BSREQ(id),
	BSREQ(types), BSREQ(levelup), BSREQ(maximum),
	{}};
BSDATAC(classi, 64)
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
BSMETA(valuei) = {
	BSREQ(id),
	BSENM(type, groupi),
	{}};
BSDATAC(valuei, 512)

#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr, 1
BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VRSTD(abilityi)},
	{"Class", VRSTD(classi)},
	{"Gender", VRSTD(genderi)},
	{"Group", VRSTD(groupi)},
	{"Special", VRSTD(speciali)},
	{"Value", VRSTD(valuei)},
};
assert_enum(varianti, Value)