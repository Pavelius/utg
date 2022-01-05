#include "bsreq.h"
#include "main.h"

#define LNK(V, T) template<> struct bsmeta<V> : bsmeta<T> {};\
template<> struct bsdata<V> : bsdata<T> {};

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSMETA(actioni) = {
	BSREQ(id),
	{}};
BSMETA(alignmenti) = {
	BSREQ(id),
	{}};
BSMETA(classi) = {
	BSREQ(id),
	BSFLG(alignment, alignmenti),
	BSFLG(gender, genderi),
	BSFLG(race, racei),
	{}};
BSDATAC(classi, 16)
BSMETA(tagi) = {
	BSREQ(id),
	{}};
BSMETA(movei) = {
	BSREQ(id),
	{}};
BSMETA(genderi) = {
	BSREQ(id),
	{}};
BSMETA(packi) = {
	BSREQ(id),
	BSREQ(elements),
	{}};
BSDATAC(packi, 256)
BSMETA(racei) = {
	BSREQ(id),
	{}};
BSMETA(startequipmenti) = {
	BSREQ(id),
	BSENM(type, classi),
	BSREQ(elements),
	{}};
BSDATAC(startequipmenti, 64)
BSMETA(itemi) = {
	BSREQ(id), BSREQ(weight), BSREQ(coins), BSREQ(damage), BSREQ(armor), BSREQ(uses), BSREQ(pierce), BSREQ(heal), BSREQ(forward),
	BSFLG(tags, tagi),
	BSFLG(moves, movei),
	{}};
BSDATAC(itemi, 256)

#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr, 1
#define VRSTN(T, N) bsmeta<T>::meta, bsdata<T>::source_ptr, N
BSDATA(varianti) = {
	{"NoVariant"},
	{"Action", VRSTD(actioni)},
	{"Class", VRSTD(classi)},
	{"Item", VRSTD(itemi)},
	{"Move", VRSTD(movei)},
	{"Pack", VRSTD(packi)},
	{"Race", VRSTD(racei)},
	{"StartEquipment", VRSTN(startequipmenti, 2)},
	{"Tag", VRSTD(tagi)},
};
assert_enum(varianti, Tag)