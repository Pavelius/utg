#include "bsreq.h"
#include "main.h"

#define LNK(V, T) template<> struct bsmeta<V> : bsmeta<T> {};\
template<> struct bsdata<V> : bsdata<T> {};

LNK(ability_s, abilityi)
LNK(gender_s, genderi)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSMETA(creature) = {
	BSREQ(id),
	BSREQ(gender),
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
	BSREQ(id),
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

#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr, 1
#define VRSTN(T, N) bsmeta<T>::meta, bsdata<T>::source_ptr, N, FG(varianti::NotFoundByName)
BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VRSTD(abilityi)},
	{"Action", VRSTD(actioni)},
	{"Advancement", VRSTN(advancement, 2)},
	{"Alignment", VRSTD(alignmenti)},
	{"Class", VRSTD(classi)},
	{"Creature", VRSTD(creature)},
	{"Diety", VRSTD(dietyi)},
	{"Front", VRSTD(front)},
	{"Gender", VRSTD(genderi)},
	{"Item", VRSTD(itemi)},
	{"Menu", VRSTD(menu)},
	{"Move", VRSTD(movei)},
	{"Pack", VRSTD(packi)},
	{"Race", VRSTD(racei)},
	{"Tag", VRSTD(tagi)},
	{"Widget", VRSTD(widget)},
};
assert_enum(varianti, Widget)