#include "action.h"
#include "card.h"
#include "bsreq.h"
#include "site.h"
#include "list.h"
#include "player.h"
#include "randomizer.h"
#include "stringbuilder.h"
#include "script.h"
#include "tactic.h"
#include "variant.h"

bool fntestlist(int index, int bonus);

NOBSDATA(point)

BSDATAC(cardi, 512)
BSDATAC(landscapei, 32)
BSDATAC(neighbor, 1024)
BSDATAC(playeri, 16)
BSDATAC(provincei, 128)
BSDATAC(sitei, 256)
BSDATAC(site, 2048)
BSDATAC(tactici, 64)

BSMETA(actioni) = {
	BSREQ(id),
	BSDST(cost, costi),
	{}};
BSMETA(costi) = {
	BSREQ(id),
	{}};
BSMETA(cardi) = {
	BSREQ(id),
	BSREQ(count),
	BSDST(effect, costi),
	BSREQ(deck),
	BSFLG(feat, feati),
	{}};
BSMETA(decki) = {
	BSREQ(id),
	{}};
BSMETA(feati) = {
	BSREQ(id),
	{}};
BSMETA(landscapei) = {
	BSREQ(id),
	BSDST(income, costi),
	BSDST(upkeep, costi),
	BSFLG(feat, feati),
	{}};
BSMETA(neighbor) = {
	BSENM(n1, provincei),
	BSENM(n2, provincei),
	{}};
BSMETA(playeri) = {
	BSREQ(id),
	BSDST(resources, costi),
	BSDST(faith, costi),
	BSREQ(start),
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
BSMETA(site) = {
	BSREQ(type),
	BSREQ(province),
	{}};
BSMETA(sitei) = {
	BSREQ(id),
	BSREQ(resid),
	BSDST(income, costi),
	BSDST(upkeep, costi),
	BSDST(cost, costi),
	BSFLG(feat, feati),
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
	{"Card", VAR(cardi, 1)},
	{"Landscape", VAR(landscapei, 1)},
	{"List", VAR(listi, 1), 0, 0, fnscript<listi>, fntestlist},
	{"Neighbor", VAR(neighbor, 2)},
	{"Player", VAR(playeri, 1)},
	{"PlayerSite", VAR(site, 0)},
	{"Province", VAR(provincei, 1), ftinfo<provincei>},
	{"RandomList", VAR(randomizeri, 1)},
	{"Script", VAR(script, 1), 0, 0, fnscript<script>},
	{"Site", VAR(sitei, 1), 0, 0, fnscript<sitei>},
	{"Tactic", VAR(tactici, 1)},
	{"Tag", VAR(tagi, 1)},
};
BSDATAF(varianti)