#include "bsreq.h"
#include "main.h"

NOBSDATA(dice)
NOBSDATA(variants)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSDATAC(bonusi, 256)
BSDATAC(classi, 16)
BSDATAC(creature, 256)
BSDATAC(itemi, 256)
BSDATAC(monsteri, 128)
BSDATAC(durationi, 32)
BSDATAC(ongoing, 128)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(feati) = {
	BSREQ(id),
	{}};
BSMETA(classi) = {
	BSREQ(id),
	{}};
BSMETA(dice) = {
	BSREQ(c), BSREQ(d), BSREQ(b), BSREQ(m),
	{}};
BSMETA(durationi) = {
	BSREQ(id),
	{}};
BSMETA(itemi) = {
	BSREQ(id),
	BSREQ(cost), BSREQ(weight),
	BSFLG(flags, itemfi),
	{}};
BSMETA(itemfi) = {
	BSREQ(id),
	{}};
BSMETA(monsteri) = {
	BSREQ(id),
	BSDST(abilities, abilityi),
	BSFLG(feats, feati),
	{}};
BSMETA(rangei) = {
	BSREQ(id),
	{}};
BSMETA(spelli) = {
	BSREQ(id),
	BSREQ(level),
	BSREQ(effect),
	BSENM(duration, durationi),
	BSENM(range, rangei),
	{}};

#define VAR(T, KN) bsmeta<T>::meta, bsdata<T>::source_ptr, KN
BSDATA(varianti) = {
	{"NoVariant"},
	{"Class", VAR(classi, 1)},
	{"Item", VAR(itemi, 1)},
	{"Gender", VAR(genderi, 1)},
	{"Monster", VAR(monsteri, 1)},
};
BSDATAF(varianti)