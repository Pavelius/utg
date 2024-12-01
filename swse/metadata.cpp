#include "ability.h"
#include "action.h"
#include "advance.h"
#include "area.h"
#include "bsreq.h"
#include "class.h"
#include "condition.h"
#include "creature.h"
#include "dice.h"
#include "item.h"
#include "modifier.h"
#include "prototype.h"
#include "script.h"
#include "size.h"
#include "skillroll.h"
#include "state.h"
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
	BSREQ(object),
	BSREQ(id),
	BSREQ(elements),
	{}};
BSMETA(area) = {
	BSENM(type, areai),
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
	BSREQ(ranged),
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
BSMETA(statei) = {
	BSREQ(id),
	{}};
BSMETA(weari) = {
	BSREQ(id),
	{}};
BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VAR(abilityi, 1), 0, 0, fnscript<abilityi>},
	{"Action", VAR(actioni, 1)},
	{"Advance", VAR(advancei, 2)},
	{"Area", VAR(areai, 1), 0, 0, fnscript<areai>},
	{"AreaObject", VAR(area, 0)},
	{"Class", VAR(classi, 1), 0, 0, fnscript<classi>},
	{"Condition", VAR(conditioni, 1), 0, 0, fnscript<conditioni>, fntest<conditioni>},
	{"Creature", VAR(creature, 0)},
	{"Feat", VAR(feati, 1), 0, 0, fnscript<feati>},
	{"Gender", VAR(genderi, 1), 0, 0, fnscript<genderi>},
	{"Item", VAR(itemi, 1), 0, 0, fnscript<itemi>},
	{"List", VAR(listi, 1), 0, 0, fnscript<listi>, fntest<listi>},
	{"Feat", VAR(feati, 1)},
	{"Menu", VAR(menu, 1)},
	{"Modifier", VAR(modifieri, 1), 0, 0, fnscript<modifieri>},
	{"Prototype", VAR(prototypei, 1)},
	{"Script", VAR(script, 1), 0, 0, fnscript<script>, fntest<script>},
	{"Size", VAR(sizei, 1)},
	{"Skill", VAR(skilli, 1), 0, 0, fnscript<skilli>},
	{"SkillRoll", VAR(skillroll, 1)},
	{"State", VAR(statei, 1), 0, 0, fnscript<statei>},
	{"Wear", VAR(weari, 1), 0, 0, fnscript<weari>},
	{"Widget", VAR(widget, 1)},
};
BSDATAF(varianti)