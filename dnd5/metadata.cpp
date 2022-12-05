#include "ability.h"
#include "alignment.h"
#include "bsreq.h"
#include "item.h"
#include "list.h"
#include "modifier.h"
#include "main.h"

#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr

NOBSDATA(dice)

BSDATAC(advancei, 512)
BSDATAC(character, 128)
BSDATAC(classi, 32)
BSDATAC(itemi, 256)
BSDATAC(languagei, 32)
BSDATAC(listi, 32)
BSDATAC(racei, 32)
BSDATAC(skilli, 32)
BSDATAC(spelli, 512)
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
BSMETA(alignmenti) = {
	BSREQ(id),
	{}};
BSMETA(character) = {
	BSREQ(id),
	{}};
BSMETA(classi) = {
	BSREQ(id),
	BSREQ(hd),
	{}};
BSMETA(damagei) = {
	BSREQ(id),
	{}};
BSMETA(dice) = {
	BSREQ(c), BSREQ(d), BSREQ(b),
	{}};
BSMETA(itemi) = {
	BSREQ(id),
	BSREQ(damage),
	{}};
BSMETA(languagei) = {
	BSREQ(id),
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
BSMETA(spelli) = {
	BSREQ(id),
	BSREQ(level),
	{}};
BSMETA(tagi) = {
	BSREQ(id),
	{}};

BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VRSTD(abilityi), 1},
	{"Advance", VRSTD(advancei), 3},
	{"Alignment", VRSTD(alignmenti), 1},
	{"Character", VRSTD(character), 1},
	{"Class", VRSTD(classi), 1},
	{"Damage", VRSTD(damagei), 1},
	{"Gender", VRSTD(genderi), 1},
	{"Item", VRSTD(itemi), 1},
	{"Language", VRSTD(languagei), 1},
	{"List", VRSTD(listi), 1},
	{"Menu", VRSTD(menu), 1},
	{"Modifier", VRSTD(modifieri), 1},
	{"Race", VRSTD(racei), 1},
	{"Skill", VRSTD(skilli), 1},
	{"Spell", VRSTD(spelli), 1},
	{"Tag", VRSTD(tagi), 1},
};
BSDATAF(varianti)