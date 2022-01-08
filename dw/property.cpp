#include "bsreq.h"
#include "main.h"

#define LNK(V, T) template<> struct bsmeta<V> : bsmeta<T> {};\
template<> struct bsdata<V> : bsdata<T> {};

LNK(ability_s, abilityi)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSMETA(creature) = {
	BSREQ(id),
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
	BSFLG(alignment, alignmenti),
	BSFLG(gender, genderi),
	BSFLG(race, racei),
	{}};
BSDATAC(classi, 16)
BSMETA(tagi) = {
	BSREQ(id),
	{}};
BSMETA(dietyi) = {
	BSREQ(id),
	{}};
BSDATAC(dietyi, 32)
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
BSMETA(startequipmenti) = {
	BSENM(type, classi),
	BSREQ(id),
	BSREQ(elements),
	{}};
BSDATAC(startequipmenti, 64)
BSMETA(itemi) = {
	BSREQ(id), BSREQ(weight), BSREQ(coins), BSREQ(damage), BSREQ(armor), BSREQ(uses), BSREQ(pierce), BSREQ(heal), BSREQ(forward),
	BSFLG(tags, tagi),
	BSFLG(moves, movei),
	{}};
BSDATAC(itemi, 256)

#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr, 1
#define VRSTN(T, N) bsmeta<T>::meta, bsdata<T>::source_ptr, N
BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VRSTD(abilityi)},
	{"Action", VRSTD(actioni)},
	{"Class", VRSTD(classi)},
	{"Creature", VRSTD(creature)},
	{"Diety", VRSTD(dietyi)},
	{"Item", VRSTD(itemi)},
	{"Move", VRSTD(movei)},
	{"Pack", VRSTD(packi)},
	{"Race", VRSTD(racei)},
	{"StartEquipment", VRSTN(startequipmenti, 2)},
	{"Tag", VRSTD(tagi)},
};
assert_enum(varianti, Tag)