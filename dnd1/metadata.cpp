#include "bsreq.h"
#include "main.h"

NOBSDATA(dice)
NOBSDATA(variants)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSDATAC(classi, 16)
BSDATAC(monsteri, 128)
BSDATAC(durationi, 32)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(dice) = {
	BSREQ(c), BSREQ(d), BSREQ(b), BSREQ(m),
	{}};
BSMETA(durationi) = {
	BSREQ(id),
	{}};
BSMETA(monsteri) = {
	BSREQ(id),
	BSDST(abilities, abilityi),
	{}};
BSMETA(spelli) = {
	BSREQ(id),
	BSREQ(damage), BSREQ(damage_per_level),
	BSREQ(level_progress),
	BSENM(duration, durationi),
	BSREQ(range),
	{}};

#define VAR(T, KN) bsmeta<T>::meta, bsdata<T>::source_ptr, KN
BSDATA(varianti) = {
	{"NoVariant"},
	{"Monster", VAR(monsteri, 1)},
};
BSDATAF(varianti)