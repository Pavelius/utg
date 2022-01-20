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
BSMETA(squadi) = {
	BSREQ(id),
	BSREQ(injury), BSREQ(gear), BSREQ(morale), BSREQ(exhause),
	{}};
BSDATAC(squadi, 128)

#define VRSTD(T, KN) bsmeta<T>::meta, bsdata<T>::source_ptr, KN
BSDATA(varianti) = {
	{"NoVariant"},
	{"Effect", VRSTD(effecti, 1)},
	{"Squad", VRSTD(squadi, 1)},
};
assert_enum(varianti, Squad)