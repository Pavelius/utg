#include "bsreq.h"
#include "card.h"
#include "component.h"
#include "crt.h"
#include "list.h"
#include "player.h"
#include "province.h"
#include "script.h"
#include "strategy.h"
#include "tag.h"
#include "troop.h"
#include "unit.h"

BSDATAC(cardi, 512)
BSDATAC(playeri, 32)
BSDATAC(provincei, 128)
BSDATAC(strategyi, 8)
BSDATAC(troopi, 128 * 8)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(cardi) = {
	BSREQ(id),
	BSENM(type, componenti),
	BSREQ(player),
	BSFLG(tags, tagi),
	BSREQ(effect),
	{}};
BSMETA(componenti) = {
	BSREQ(id),
	{}};
BSMETA(playeri) = {
	BSREQ(id),
	{}};
BSMETA(provincei) = {
	BSREQ(id),
	BSREQ(player),
	{}};
BSMETA(strategyi) = {
	BSREQ(id),
	BSREQ(primary), BSREQ(secondary),
	{}};
BSMETA(tagi) = {
	BSREQ(id),
	{}};
BSMETA(uniti) = {
	BSREQ(id),
	BSDST(abilities, abilityi),
	BSREQ(combat),
	BSFLG(tags, tagi),
	{}};
BSDATA(varianti) = {
	{"Ability", VAR(abilityi, 1), 0, 0, fnscript<abilityi>},
	{"Card", VAR(cardi, 1)},
	{"Player", VAR(playeri, 1)},
	{"List", VAR(listi, 1), 0, 0, fnscript<listi>},
	{"Script", VAR(script, 1), 0, 0, fnscript<script>, fntest<script>},
	{"Strategy", VAR(strategyi, 1), ftinfo<strategyi>},
	{"Unit", VAR(uniti, 1), 0, 0, fnscript<uniti>},
};
BSDATAF(varianti);