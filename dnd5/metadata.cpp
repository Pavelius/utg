#include "bsreq.h"
#include "main.h"

#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr

NOBSDATA(dice)
NOBSDATA(itemi::weaponi)

BSDATAC(advancei, 512)
BSDATAC(classi, 32)
BSDATAC(itemi, 256)
BSDATAC(languagei, 32)
BSDATAC(listi, 32)
BSDATAC(racei, 32)
BSDATAC(skilli, 32)
BSDATAD(variant)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(advancei) = {
	BSREQ(id),
	BSREQ(base),
	BSREQ(level),
	BSREQ(choose),
	BSREQ(effect),
	{}};
BSMETA(classi) = {
	BSREQ(id),
	{}};
BSMETA(damagei) = {
	BSREQ(id),
	{}};
BSMETA(dice) = {
	BSREQ(c), BSREQ(d), BSREQ(b),
	{}};
BSMETA(itemi::weaponi) = {
	BSREQ(damage),
	{}};
BSMETA(itemi) = {
	BSREQ(id),
	BSREQ(weapon),
	{}};
BSMETA(languagei) = {
	BSREQ(id),
	{}};
BSMETA(listi) = {
	BSREQ(id),
	BSREQ(elements),
	{}};
BSMETA(modifieri) = {
	BSREQ(id),
	{}};
BSMETA(racei) = {
	BSREQ(id),
	BSREQ(parent),
	{}};
BSMETA(skilli) = {
	BSREQ(id),
	{}};
BSMETA(tagi) = {
	BSREQ(id),
	{}};

BSDATA(varianti) = {
	{"Ability", VRSTD(abilityi), 1},
	{"Advance", VRSTD(advancei), 3},
	{"Class", VRSTD(classi), 1},
	{"Damage", VRSTD(damagei), 1},
	{"Item", VRSTD(itemi), 1},
	{"Language", VRSTD(languagei), 1},
	{"List", VRSTD(listi), 1},
	{"Modifier", VRSTD(modifieri), 1},
	{"Race", VRSTD(racei), 1},
	{"Skill", VRSTD(skilli), 1},
	{"Tag", VRSTD(tagi), 1},
};
BSDATAF(varianti)