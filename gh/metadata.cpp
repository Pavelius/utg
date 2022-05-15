#include "bsreq.h"
#include "main.h"

NOBSDATA(point)
NOBSDATA(scenariotilei)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)
BSDATAC(summoni, 64)
BSDATAC(playeri, 24)
BSDATAC(playercardi, 512)
BSDATAC(scenarioi, 128)

#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr

BSMETA(point) = {
	BSREQ(x), BSREQ(y),
	{}};
BSMETA(actioni) = {
	BSREQ(id),
	{}};
BSMETA(areai) = {
	BSREQ(id),
	{}};
BSMETA(durationi) = {
	BSREQ(id),
	{}};
BSMETA(elementi) = {
	BSREQ(id),
	{}};
BSMETA(genderi) = {
	BSREQ(id),
	{}};
BSMETA(modifieri) = {
	BSREQ(id),
	{}};
BSMETA(playeri) = {
	BSREQ(id),
	BSREQ(health),
	BSENM(gender, genderi),
	{}};
BSMETA(playercardi) = {
	BSREQ(id),
	BSREQ(level),
	BSREQ(initiative),
	BSREQ(owner),
	BSREQ(upper), BSREQ(lower),
	{}};
BSMETA(scripti) = {
	BSREQ(id),
	{}};
BSMETA(tilei) = {
	BSREQ(id),
	{}};
BSMETA(speciali) = {
	BSREQ(id),
	{}};
BSMETA(statei) = {
	BSREQ(id),
	{}};
BSMETA(scenariotilei) = {
	BSREQ(type),
	BSREQ(position),
	BSREQ(inverse),
	{}};
BSMETA(scenarioi) = {
	BSREQ(id),
	BSREQ(tiles),
	{}};
BSMETA(summoni) = {
	BSREQ(id),
	BSREQ(hits), BSREQ(move), BSREQ(attack), BSREQ(range),
	{}};

BSDATA(varianti) = {
	{"NoVariant"},
	{"Action", VRSTD(actioni), 1},
	{"Area", VRSTD(areai), 1},
	{"Condition", VRSTD(conditioni), 1},
	{"Duration", VRSTD(durationi), 1},
	{"Element", VRSTD(elementi), 1},
	{"Gender", VRSTD(genderi), 1},
	{"Modifier", VRSTD(modifieri), 1},
	{"Player", VRSTD(playeri), 1},
	{"PlayerCard", VRSTD(playercardi), 1},
	{"Scenario", VRSTD(scenarioi), 1},
	{"Script", VRSTD(scripti), 1},
	{"Special", VRSTD(speciali), 1},
	{"State", VRSTD(statei), 1},
	{"Summon", VRSTD(summoni), 1},
};
BSDATAF(varianti)
