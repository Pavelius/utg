#include "action.h"
#include "bsreq.h"
#include "building.h"
#include "hero.h"
#include "list.h"
#include "player.h"
#include "randomizer.h"
#include "stringbuilder.h"
#include "script.h"
#include "tactic.h"
#include "tag.h"
#include "variant.h"

bool fntestlist(int index, int bonus);

NOBSDATA(point)

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
	BSREQ(player),
	BSREQ(wounds),
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
	BSREQ(builded), BSREQ(recruit), BSREQ(units),
	{}};
BSMETA(site) = {
	BSREQ(type),
	BSREQ(province),
	{}};
BSMETA(sitei) = {
	BSREQ(id),
	BSREQ(resid),
	BSREQ(effect),
	BSREQ(conditions), BSREQ(fail), BSREQ(partial), BSREQ(success),
	BSREQ(upkeep),
	{}};
BSMETA(tactici) = {
	BSREQ(id),
	BSDST(effect, costi),
	{}};
BSMETA(tagi) = {
	BSREQ(id),
	{}};
BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VAR(costi, 1), ftinfo<costi>, 0, fnscript<costi>},
	{"Action", VAR(actioni, 1)},
	{"Building", VAR(buildingi, 1), ftinfo<buildingi>, 0, fnscript<buildingi>},
	{"Hero", VAR(heroi, 1)},
	{"Landscape", VAR(landscapei, 1)},
	{"List", VAR(listi, 1), 0, 0, fnscript<listi>, fntestlist},
	{"Neighbor", VAR(neighbor, 2)},
	{"Player", VAR(playeri, 1)},
	{"PlayerBuilding", VAR(building, 0), ftinfo<building>},
	{"PlayerSite", VAR(site, 0)},
	{"Province", VAR(provincei, 1), ftinfo<provincei>},
	{"RandomList", VAR(randomizeri, 1)},
	{"Script", VAR(script, 1), 0, 0, fnscript<script>},
	{"Site", VAR(sitei, 1)},
	{"Tactic", VAR(tactici, 1)},
	{"Tag", VAR(tagi, 1)},
};
BSDATAF(varianti)