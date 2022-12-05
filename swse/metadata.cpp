#include "ability.h"
#include "advance.h"
#include "bsreq.h"
#include "classi.h"
#include "crt.h"
#include "creature.h"
#include "dice.h"
#include "item.h"
#include "modifier.h"
#include "size.h"
#include "skillroll.h"
#include "wear.h"
#include "utg.h"

NOBSDATA(dice)
NOBSDATA(variants)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSDATAC(advancei, 256)
BSDATAC(classi, 16)
BSDATAC(creature, 1024)
BSDATAC(feati, 512)
BSDATAC(itemi, 256)
BSDATAC(skilli, 32)
BSDATAC(skillroll, 128)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(advancei) = {
	BSREQ(level),
	BSREQ(object),
	BSREQ(id),
	BSREQ(elements),
	{}};
BSMETA(classi) = {
	BSREQ(id),
	BSFLG(skills, skilli),
	BSREQ(hd), BSREQ(force), BSREQ(skill),
	{}};
BSMETA(creature) = {
	{}};
BSMETA(damagei) = {
	BSREQ(id),
	{}};
BSMETA(dice) = {
	BSREQ(c), BSREQ(d), BSREQ(b), BSREQ(m),
	{}};
BSMETA(feati) = {
	BSREQ(id),
	BSREQ(prerequisit),
	BSREQ(effect),
	{}};
BSMETA(itemi) = {
	BSREQ(id), BSREQ(cost), BSREQ(weight),
	BSENM(hit, damagei),
	BSREQ(damage), BSREQ(stun),
	BSENM(size, sizei),
	BSENM(wear, weari),
	BSREQ(use),
	BSFLG(feats, feati),
	{}};
BSMETA(modifieri) = {
	BSREQ(id),
	{}};
BSMETA(sizei) = {
	BSREQ(id),
	{}};
BSMETA(skilli) = {
	BSREQ(id),
	BSENM(ability, abilityi),
	BSFLG(reroll, feati),
	{}};
BSMETA(skillroll) = {
	BSREQ(id),
	BSENM(skill, skilli),
	BSFLG(reroll, feati),
	BSREQ(bonuses), BSREQ(required),
	{}};
BSMETA(weari) = {
	BSREQ(id),
	{}};
BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VAR(abilityi, 1)},
	{"Advance", VAR(advancei, 3)},
	{"Class", VAR(classi, 1)},
	{"Creature", VAR(creature, 0)},
	{"Feat", VAR(feati, 1)},
	{"Item", VAR(itemi, 1)},
	{"List", VAR(listi, 1)},
	{"Feat", VAR(feati, 1)},
	{"Menu", VAR(menu, 1)},
	{"Modifier", VAR(modifieri, 1)},
	{"Size", VAR(sizei, 1)},
	{"Skill", VAR(skilli, 1)},
	{"SkillRoll", VAR(skillroll, 1)},
	{"Widget", VAR(widget, 1)},
};
BSDATAF(varianti)