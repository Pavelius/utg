#include "ability.h"
#include "advance.h"
#include "alignment.h"
#include "bsreq.h"
#include "character.h"
#include "class.h"
#include "damage.h"
#include "item.h"
#include "language.h"
#include "list.h"
#include "menu.h"
#include "modifier.h"
#include "option.h"
#include "race.h"
#include "script.h"
#include "skill.h"
#include "spell.h"
#include "tag.h"
#include "widget.h"

NOBSDATA(dice)

BSDATAC(advancei, 1024)
BSDATAC(character, 128)
BSDATAC(classi, 32)
BSDATAC(itemi, 256)
BSDATAC(languagei, 32)
BSDATAC(listi, 32)
BSDATAC(optioni, 120)
BSDATAC(racei, 32)
BSDATAC(skilli, 32)
BSDATAC(spelli, 512)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(advancei) = {
	BSREQ(level),
	BSREQ(object),
	BSREQ(id),
	BSREQ(choose),
	BSREQ(columns),
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
BSMETA(optioni) = {
	BSREQ(id),
	BSREQ(parent),
	BSREQ(effect),
	BSREQ(spells),
	{}};
BSMETA(racei) = {
	BSREQ(id),
	BSREQ(parent),
	{}};
BSMETA(skilli) = {
	BSREQ(id),
	BSENM(ability, abilityi),
	{}};
BSMETA(spelli) = {
	BSREQ(id),
	BSREQ(level),
	BSDST(classes, classi),
	{}};
BSMETA(tagi) = {
	BSREQ(id),
	{}};

BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VAR(abilityi, 1), 0, 0, fnscript<abilityi>},
	{"Advance", VAR(advancei, 3)},
	{"Alignment", VAR(alignmenti, 1)},
	{"Character", VAR(character, 1)},
	{"Class", VAR(classi, 1)},
	{"Damage", VAR(damagei, 1), 0, 0, fnscript<damagei>, fntest<damagei>},
	{"Gender", VAR(genderi, 1)},
	{"Item", VAR(itemi, 1), 0, 0, fnscript<itemi>, fntest<itemi>},
	{"Language", VAR(languagei, 1)},
	{"List", VAR(listi, 1), 0, 0, fnscript<listi>},
	{"Menu", VAR(menu, 1)},
	{"Modifier", VAR(modifieri, 1), 0, 0, fnscript<modifieri>},
	{"Option", VAR(optioni, 1)},
	{"Race", VAR(racei, 1)},
	{"Script", VAR(script, 1), 0, 0, fnscript<script>},
	{"Skill", VAR(skilli, 1), 0, 0, fnscript<skilli>, fntest<skilli>},
	{"Spell", VAR(spelli, 1), 0, 0, fnscript<spelli>, fntest<spelli>},
	{"Tag", VAR(tagi, 1)},
	{"Widget", VAR(widget, 1)},
};
BSDATAF(varianti)