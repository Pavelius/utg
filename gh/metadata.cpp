#include "bsreq.h"
#include "main.h"

NOBSDATA(color)
NOBSDATA(point)
NOBSDATA(scenariotilei)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)
BSDATAC(activecardi, 32)
BSDATAC(combatcardi, 512)
BSDATAC(monsteri, 256)
BSDATAC(monstercardi, 256)
BSDATAC(playeri, 24)
BSDATAC(playercardi, 512)
BSDATAC(scenarioi, 128)
BSDATAC(summoni, 64)
BSDATAC(decoration, 128)
BSDATAC(trapi, 128)

#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr

BSMETA(point) = {
	BSREQ(x), BSREQ(y),
	{}};
BSMETA(color) = {
	BSREQ(r), BSREQ(g), BSREQ(b),
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
	BSREQ(fore),
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
	BSREQ(tiles), BSREQ(starts),
	{}};
BSMETA(summoni) = {
	BSREQ(id),
	BSREQ(hits), BSREQ(move), BSREQ(attack), BSREQ(range),
	{}};
BSMETA(monsteri) = {
	BSREQ(id), BSREQ(level), BSREQ(elite),
	BSREQ(hits), BSREQ(move), BSREQ(attack), BSREQ(range),
	BSREQ(feats), BSREQ(deck),
	{}};
BSMETA(monstercardi) = {
	BSREQ(id),
	BSREQ(initiative), BSREQ(shuffle),
	BSREQ(abilities),
	{}};
BSMETA(combatcardi) = {
	BSREQ(id),
	BSREQ(bonus), BSREQ(shuffle), BSREQ(next), BSREQ(count),
	BSREQ(feats),
	{}};
BSMETA(targeti) = {
	BSREQ(id),
	{}};
BSMETA(trapi) = {
	BSREQ(id),
	BSREQ(damage),
	{}};

BSDATA(varianti) = {
	{"NoVariant"},
	{"Action", VRSTD(actioni), 1},
	{"Area", VRSTD(areai), 1},
	{"Condition", VRSTD(conditioni), 1},
	{"CombatCard", VRSTD(combatcardi), 1},
	{"Duration", VRSTD(durationi), 1},
	{"Element", VRSTD(elementi), 1},
	{"Gender", VRSTD(genderi), 1},
	{"Modifier", VRSTD(modifieri), 1},
	{"Monster", VRSTD(monsteri), 3},
	{"MonsterCard", VRSTD(monstercardi), 0},
	{"Player", VRSTD(playeri), 1},
	{"PlayerCard", VRSTD(playercardi), 1},
	{"Scenario", VRSTD(scenarioi), 1},
	{"Script", VRSTD(scripti), 1},
	{"Special", VRSTD(speciali), 1},
	{"State", VRSTD(statei), 1},
	{"Summon", VRSTD(summoni), 1},
	{"Target", VRSTD(targeti), 1},
	{"Trap", VRSTD(trapi), 1},
};
BSDATAF(varianti)
