#include "action.h"
#include "bsreq.h"
#include "building.h"
#include "hero.h"
#include "list.h"
#include "player.h"
#include "randomizer.h"
#include "script.h"
#include "tactic.h"
#include "tag.h"
#include "unit.h"
#include "variant.h"

bool fntestlist(int index, int bonus);

NOBSDATA(point)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)
BSDATAC(buildingi, 128)
BSDATAC(building, 1024)
BSDATAC(heroi, 64)
BSDATAC(landscapei, 32)
BSDATAC(neighbor, 1024)
BSDATAC(playeri, 16)
BSDATAC(provincei, 128)
BSDATAC(sitei, 256)
BSDATAC(site, 2048)
BSDATAC(tactici, 64)
BSDATAC(troop, 1024)
BSDATAC(uniti, 48)

BSMETA(actioni) = {
	BSREQ(id),
	{}};
BSMETA(buildingi) = {
	BSREQ(id),
	BSDST(effect, costi),
	BSREQ(upgrade),
	BSDST(upkeep, costi),
	BSDST(cost, costi),
	BSREQ(conditions),
	{}};
BSMETA(building) = {
	BSREQ(type),
	BSREQ(province),
	{}};
BSMETA(costi) = {
	BSREQ(id),
	{}};
BSMETA(heroi) = {
	BSREQ(id),
	BSREQ(resid),
	BSDST(effect, costi),
	BSDST(cost, costi),
	BSDST(upkeep, costi),
	BSREQ(player),
	{}};
BSMETA(landscapei) = {
	BSREQ(id), BSREQ(water),
	BSDST(effect, costi),
	BSDST(upkeep, costi),
	{}};
BSMETA(neighbor) = {
	BSENM(n1, provincei),
	BSENM(n2, provincei),
	{}};
BSMETA(playeri) = {
	BSREQ(id),
	BSDST(resources, costi),
	BSDST(income, costi),
	BSREQ(start),
	BSFLG(tactics, tactici),
	{}};
BSMETA(point) = {
	BSREQ(x), BSREQ(y),
	{}};
BSMETA(provincei) = {
	BSREQ(id),
	BSREQ(player),
	BSREQ(position),
	BSREQ(landscape),
	BSDST(income, costi),
	BSREQ(builded), BSREQ(recruit),
	{}};
BSMETA(script) = {
	BSREQ(id),
	{}};
BSMETA(site) = {
	BSREQ(type),
	BSREQ(province),
	BSFLG(tags, tagi),
	{}};
BSMETA(sitei) = {
	BSREQ(id),
	BSREQ(resid),
	BSREQ(effect),
	BSREQ(conditions), BSREQ(defenders), BSREQ(reward),
	BSREQ(upkeep),
	{}};
BSMETA(tactici) = {
	BSREQ(id),
	BSDST(effect, costi),
	BSDST(perunit, costi),
	BSDST(cost, costi),
	BSREQ(upgrade),
	BSFLG(disable, tactici),
	{}};
BSMETA(tagi) = {
	BSREQ(id),
	{}};
BSMETA(troop) = {
	BSREQ(type),
	BSREQ(province), BSREQ(moveto), BSREQ(moveto_location),
	BSREQ(player),
	{}};
BSMETA(uniti) = {
	BSREQ(id),
	BSREQ(fame),
	BSDST(effect, costi),
	BSDST(cost, costi),
	BSDST(upkeep, costi),
	BSFLG(tactics, tactici),
	BSFLG(tags, tagi),
	{}};
BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VAR(costi, 1), ftstatus<costi>},
	{"Action", VAR(actioni, 1)},
	{"Building", VAR(buildingi, 1), ftstatus<buildingi>, 0, fnscript<buildingi>},
	{"Hero", VAR(heroi, 1)},
	{"Landscape", VAR(landscapei, 1)},
	{"List", VAR(listi, 1), 0, 0, fnscript<listi>, fntestlist},
	{"Neighbor", VAR(neighbor, 2)},
	{"Player", VAR(playeri, 1)},
	{"PlayerBuilding", VAR(building, 0), ftstatus<building>},
	{"PlayerSite", VAR(site, 0)},
	{"Province", VAR(provincei, 1), ftstatus<provincei>},
	{"RandomList", VAR(randomizeri, 1)},
	{"Script", VAR(script, 1), 0, 0, fnscript<script>},
	{"Site", VAR(sitei, 1)},
	{"Tactic", VAR(tactici, 1)},
	{"Tag", VAR(tagi, 1)},
	{"Troop", VAR(troop, 1), ftstatus<troop>},
	{"Unit", VAR(uniti, 1), ftstatus<uniti>, 0, fnscript<uniti>},
};
BSDATAF(varianti)