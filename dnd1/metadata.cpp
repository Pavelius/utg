#include "bsreq.h"
#include "main.h"

NOBSDATA(dice)
NOBSDATA(itemi::armori)
NOBSDATA(itemi::weaponi)
NOBSDATA(variants)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSDATAC(classi, 16)
BSDATAC(creature, 256)
BSDATAC(durationi, 32)
BSDATAC(enchantmenti, 256)
BSDATAC(enchantmentseti, 64)
BSDATAC(equipmenti, 256)
BSDATAC(itemi, 256)
BSDATAC(monsteri, 128)
BSDATAC(ongoing, 256)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(classi) = {
	BSREQ(id),
	{}};
BSMETA(creature) = {
	BSDST(abilities, abilityi),
	{}};
BSMETA(dice) = {
	BSREQ(c), BSREQ(d), BSREQ(b), BSREQ(m),
	{}};
BSMETA(durationi) = {
	BSREQ(id),
	{}};
BSMETA(enchantmenti) = {
	BSREQ(special),
	BSREQ(title),
	{}};
BSMETA(enchantmentseti) = {
	BSREQ(id),
	BSREQ(elements),
	{}};
BSMETA(equipmenti) = {
	BSENM(type, classi),
	BSENM(equipment, itemi),
	{}};
BSMETA(feati) = {
	BSREQ(id),
	{}};
BSMETA(itemi) = {
	BSREQ(id),
	BSREQ(cost), BSREQ(weight), BSREQ(count),
	BSFLG(flags, feati),
	BSENM(wear, weari),
	BSREQ(weapon),
	BSREQ(armor),
	BSREQ(enchantments),
	{}};
BSMETA(itemi::weaponi) = {
	BSREQ(damage),
	BSENM(ammunition, itemi),
	{}};
BSMETA(itemi::armori) = {
	BSREQ(ac),
	{}};
BSMETA(monsteri) = {
	BSREQ(id),
	BSDST(abilities, abilityi),
	BSFLG(feats, feati),
	BSREQ(dungeon),
	BSREQ(wilderness),
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
BSMETA(weari) = {
	BSREQ(id),
	{}};

#define VAR(T) bsmeta<T>::meta, bsdata<T>::source_ptr
BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VAR(abilityi), 1},
	{"Class", VAR(classi), 1},
	{"Creature", VAR(creature), 0, 0, creature::getstatus, creature::getproperty},
	{"Equipment", VAR(equipmenti), 2},
	{"Enchantment", VAR(enchantmenti), 3},
	{"EnchantmentSet", VAR(enchantmentseti), 1},
	{"Gender", VAR(genderi), 1},
	{"Item", VAR(itemi), 1},
	{"List", VAR(listi), 1},
	{"Menu", VAR(menu), 1},
	{"Monster", VAR(monsteri), 1},
	{"Spell", VAR(spelli), 1},
	{"Widget", VAR(widget), 1},
};
BSDATAF(varianti)