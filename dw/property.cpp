#include "bsreq.h"
#include "main.h"
#include "widget.h"

#define LNK(V, T) template<> struct bsmeta<V> : bsmeta<T> {};\
template<> struct bsdata<V> : bsdata<T> {};

LNK(ability_s, abilityi)
LNK(gender_s, genderi)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSMETA(creature) = {
	BSENM(type, classi),
	BSENM(race, racei),
	BSENM(alignment, alignmenti),
	BSENM(diety, dietyi),
	{}};
BSDATAC(creature, 4)
BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(actioni) = {
	BSREQ(id),
	{}};
BSMETA(alignmenti) = {
	BSREQ(id),
	{}};
BSMETA(classi) = {
	BSREQ(id),
	BSREQ(abilities),
	BSREQ(damage),
	{}};
BSDATAC(classi, 16)
BSMETA(tagi) = {
	BSREQ(id),
	{}};
BSMETA(dietyi) = {
	BSREQ(id),
	{}};
BSDATAC(dietyi, 32)
BSMETA(menu) = {
	BSREQ(id), BSREQ(source),
	BSREQ(elements),
	{}};
BSDATAC(menu, 256)
BSMETA(movei) = {
	BSREQ(id),
	{}};
BSMETA(genderi) = {
	BSREQ(id),
	{}};
BSMETA(packi) = {
	BSREQ(id),
	BSREQ(elements),
	{}};
BSDATAC(packi, 256)
BSMETA(racei) = {
	BSREQ(id),
	{}};
BSMETA(itemi) = {
	BSREQ(id), BSREQ(weight), BSREQ(coins), BSREQ(damage), BSREQ(armor), BSREQ(uses), BSREQ(pierce), BSREQ(heal), BSREQ(forward),
	BSFLG(tags, tagi),
	BSFLG(moves, movei),
	{}};
BSDATAC(itemi, 256)

#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr
BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VRSTD(abilityi), 1},
	{"Action", VRSTD(actioni), 1},
	{"Advancement", VRSTD(advancement), 2},
	{"Alignment", VRSTD(alignmenti), 1},
	{"Class", VRSTD(classi), 1},
	{"Creature", VRSTD(creature)},
	{"Diety", VRSTD(dietyi), 1},
	{"Front", VRSTD(front), 1},
	{"Gender", VRSTD(genderi), 1},
	{"Item", VRSTD(itemi), 1},
	{"Menu", VRSTD(menu), 1},
	{"Move", VRSTD(movei), 1},
	{"Pack", VRSTD(packi), 1},
	{"Race", VRSTD(racei), 1},
	{"Tag", VRSTD(tagi), 1},
	{"Widget", VRSTD(widget), 1},
};
assert_enum(varianti, Widget)