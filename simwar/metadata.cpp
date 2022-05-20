#include "ability.h"
#include "bsreq.h"
#include "resource.h"
#include "variant.h"

#define LNK(V, T) template<> struct bsmeta<V> : bsmeta<T> {};\
template<> struct bsdata<V> : bsdata<T> {};

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSDATAC(abilityi, 32)
BSMETA(resourcei) = {
	BSREQ(id),
	BSREQ(format),
	{}};
BSDATAC(resourcei, 32)

#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr
BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VRSTD(abilityi), 1},
	{"Resource", VRSTD(resourcei), 1},
};
BSDATAF(varianti)