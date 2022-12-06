#include "ability.h"
#include "advance.h"
#include "alignment.h"
#include "class.h"
#include "bsreq.h"
#include "damage.h"
#include "item.h"
#include "language.h"
#include "list.h"
#include "menu.h"
#include "modifier.h"
#include "character.h"
#include "race.h"
#include "script.h"
#include "skill.h"
#include "spell.h"
#include "tag.h"

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
	BSREQ(level),
	BSREQ(object),
	BSREQ(id),
	BSREQ(choose),
	BSREQ(elements),
	{}};
BSMETA(alignmenti) = {
	BSREQ(id),
	{}};
BSMETA(character) = {
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
BSMETA(script) = {
	BSREQ(id),
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
	{"Ability", VAR(abilityi, 1)},
	{"Advance", VAR(advancei, 3)},
	{"Alignment", VAR(alignmenti, 1)},
	{"Character", VAR(character, 1)},
	{"Class", VAR(classi, 1)},
	{"Damage", VAR(damagei, 1)},
	{"Gender", VAR(genderi, 1)},
	{"Item", VAR(itemi, 1)},
	{"Language", VAR(languagei, 1)},
	{"List", VAR(listi, 1)},
	{"Menu", VAR(menu, 1)},
	{"Modifier", VAR(modifieri, 1)},
	{"Race", VAR(racei, 1)},
	{"Script", VAR(script, 1)},
	{"Skill", VAR(skilli, 1)},
	{"Spell", VAR(spelli, 1)},
	{"Tag", VAR(tagi, 1)},
};
BSDATAF(varianti)