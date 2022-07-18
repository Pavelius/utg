#include "bsreq.h"
#include "main.h"
#include "variant.h"

#define LNK(V, T) template<> struct bsmeta<V> : bsmeta<T> {};\
template<> struct bsdata<V> : bsdata<T> {};

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSMETA(stati) = {
	BSREQ(id),
	BSREQ(format),
	{}};

#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr
BSDATA(varianti) = {
	{"NoVariant"},
	{"Stat", VRSTD(stati), 1},
};
BSDATAF(varianti)