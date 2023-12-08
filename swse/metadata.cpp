#include "ability.h"
#include "action.h"
#include "advance.h"
#include "area.h"
#include "bsreq.h"
#include "classi.h"
#include "crt.h"
#include "creature.h"
#include "dice.h"
#include "item.h"
#include "modifier.h"
#include "prototype.h"
#include "script.h"
#include "size.h"
#include "skillroll.h"
#include "wear.h"
#include "draw_utg.h"

NOBSDATA(dice)

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
BSMETA(actioni) = {
	BSREQ(id),
	BSENM(type, abilityi),
	BSREQ(effect),
	BSREQ(upgrade),
	{}};
BSMETA(advancei) = {
	BSREQ(level),
	BSREQ(object),
	BSREQ(id),
	BSREQ(elements),
	{}};
BSMETA(area) = {
	BSENM(type, areai),
	BSREQ(place),
	{}};
BSMETA(areai) = {
	BSREQ(id),
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
	BSENM(proficiency, feati),
	BSENM(focus, feati),
	BSENM(specialization, feati),
	BSREQ(use),
	{}};
BSMETA(modifieri) = {
	BSREQ(id),
	{}};
BSMETA(prototypei) = {
	BSREQ(id),
	BSREQ(tags),
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
	BSREQ(dc),
	{}};
BSMETA(weari) = {
	BSREQ(id),
	{}};
BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VAR(abilityi, 1), 0, 0, fnscript<abilityi>, fntest<abilityi>},
	{"Action", VAR(actioni, 1)},
	{"Advance", VAR(advancei, 3)},
	{"Area", VAR(areai, 1)},
	{"AreaObject", VAR(area, 0)},
	{"Class", VAR(classi, 1)},
	{"Creature", VAR(creature, 0)},
	{"Feat", VAR(feati, 1)},
	{"Item", VAR(itemi, 1)},
	{"List", VAR(listi, 1)},
	{"Feat", VAR(feati, 1)},
	{"Menu", VAR(menu, 1)},
	{"Modifier", VAR(modifieri, 1), 0, 0, fnscript<modifieri>},
	{"Prototype", VAR(prototypei, 1)},
	{"Script", VAR(script, 1), 0, 0, fnscript<script>, fntest<script>},
	{"Size", VAR(sizei, 1)},
	{"Skill", VAR(skilli, 1), 0, 0, fnscript<skilli>, fntest<skilli>},
	{"SkillRoll", VAR(skillroll, 1)},
	{"Wear", VAR(weari, 1), 0, 0, fnscript<weari>, fntest<weari>},
	{"Widget", VAR(widget, 1)},
};
BSDATAF(varianti)