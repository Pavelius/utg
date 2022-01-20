#include "bsreq.h"
#include "main.h"

#define XKIND(T, V) template<> struct bsmeta<T> : bsmeta<V> {};\
template<> struct bsdata<T> : bsdata<V> {};

NOBSDATA(variants)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSMETA(effecti) = {
	BSREQ(id),
	{}};
BSMETA(movei) = {
	BSREQ(id),
	{}};
BSMETA(resulti) = {
	BSREQ(id),
	{}};
BSMETA(squadi) = {
	BSREQ(id),
	BSFLG(tags, tagi),
	{}};
BSMETA(tagi) = {
	BSREQ(id),
	{}};

#define VRSTD(T, KN) bsmeta<T>::meta, bsdata<T>::source_ptr, KN
#define VRNOV() 0, 0, 0
BSDATA(varianti) = {
	{"NoVariant"},
	{"Effect", VRSTD(effecti, 1)},
	{"Move", VRSTD(movei, 1)},
	{"Prefix", VRNOV()},
	{"Result", VRSTD(resulti, 1)},
	{"Squad", VRSTD(squadi, 1)},
};
assert_enum(varianti, Squad)