#include "bsreq.h"
#include "main.h"

#define LNK(V, T) template<> struct bsmeta<V> : bsmeta<T> {};\
template<> struct bsdata<V> : bsdata<T> {};

LNK(ability_s, abilityi)
LNK(gender_s, genderi)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
//BSMETA(bonusi) = {
//	BSREQ(id), BSENM(object, harmi), BSREQ(value),
//	{}};
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
	BSREQ(roll),
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

#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr, 1
#define VRSTN(T, N) bsmeta<T>::meta, bsdata<T>::source_ptr, N, FG(varianti::NotFoundByName)
BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VRSTD(abilityi)},
	{"Item", VRSTD(itemi)},
	{"Menu", VRSTD(menu)},
	{"Move", VRSTD(movei)},
	{"Resource", VRSTD(harmi)},
	{"Result", VRSTD(resulti)},
	{"Result", VRSTD(riski)},
	{"Specail", VRSTD(speciali)},
	{"Tag", VRSTD(tagi)},
	{"Widget", VRSTD(widget)},
};
BSDATAF(varianti)