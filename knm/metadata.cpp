#include "bsreq.h"
#include "card.h"
#include "crt.h"
#include "deck.h"
#include "filter.h"
#include "list.h"
#include "player.h"
#include "province.h"
#include "script.h"
#include "strategy.h"
#include "structure.h"
#include "tag.h"
#include "troop.h"
#include "unit.h"
#include "upgrade.h"

void reapeated_list(int value, int counter);

NOBSDATA(color)
NOBSDATA(point)

BSDATAC(card, 1024)
BSDATAC(cardi, 512)
BSDATAC(playeri, 32)
BSDATAC(provincei, 128)
BSDATAC(structurei, 8)
BSDATAC(structure, 256)
BSDATAC(strategyi, 8)
BSDATAC(troopi, 128 * 8)
BSDATAC(upgradei, 32)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(cardi) = {
	BSREQ(id),
	BSREQ(count),
	BSREQ(player),
	BSREF(location, decki),
	BSREQ(trigger),
	BSFLG(tags, tagi),
	BSREQ(effect),
	BSREQ(target),
	BSREQ(priority),
	BSDST(abilities, abilityi),
	{}};
BSMETA(color) = {
	BSREQ(r), BSREQ(g), BSREQ(b),
	{}};
BSMETA(decki) = {
	BSREQ(id),
	{}};
BSMETA(filteri) = {
	BSREQ(id),
	{}};
BSMETA(playeri) = {
	BSREQ(id),
	BSREQ(troops),
	BSREQ(homeland),
	BSREQ(start),
	BSREQ(fore),
	BSFLG(upgrades, upgradei),
	{}};
BSMETA(point) = {
	BSREQ(x), BSREQ(y),
	{}};
BSMETA(provincei) = {
	BSREQ(id),
	BSREQ(player),
	BSREQ(points),
	BSFLG(tags, tagi),
	BSDST(abilities, abilityi),
	{}};
BSMETA(strategyi) = {
	BSREQ(id),
	BSREQ(initiative),
	BSREQ(primary), BSREQ(secondary),
	{}};
BSMETA(structurei) = {
	BSREQ(id),
	BSDST(abilities, abilityi),
	BSREQ(avatar),
	{}};
BSMETA(structure) = {
	{}};
BSMETA(tagi) = {
	BSREQ(id),
	{}};
BSMETA(troopi) = {
	{}};
BSMETA(uniti) = {
	BSREQ(id),
	BSDST(abilities, abilityi),
	BSREQ(combat),
	BSFLG(tags, tagi),
	{}};
BSMETA(upgradei) = {
	BSREQ(id),
	BSREQ(player),
	BSDST(abilities, abilityi),
	BSREQ(trigger),
	BSREQ(effect),
	{}};
BSDATA(varianti) = {
	{"Ability", VAR(abilityi, 1), 0, 0, fnscript<abilityi>},
	{"Card", VAR(cardi, 1), ftinfo<cardi>},
	{"Deck", VAR(decki, 1), 0, 0, fnscript<decki>},
	{"Filter", VAR(filteri, 1), 0, 0, fnscript<filteri>},
	{"List", VAR(listi, 1), 0, 0, reapeated_list, fntest<listi>},
	{"Player", VAR(playeri, 1)},
	{"Province", VAR(provincei, 1)},
	{"Script", VAR(script, 1), 0, 0, fnscript<script>, fntest<script>},
	{"Strategy", VAR(strategyi, 1), ftinfo<strategyi>},
	{"Structure", VAR(structurei, 1), ftinfo<structurei>, 0, fnscript<structurei>},
	{"StructureObject", VAR(structure, 0), ftinfo<structure>},
	{"Tag", VAR(tagi, 1), 0, 0, fnscript<tagi>, fntest<tagi>},
	{"Unit", VAR(uniti, 1), 0, 0, fnscript<uniti>},
	{"UnitObject", VAR(troopi, 0), ftinfo<troopi>},
	{"Upgrade", VAR(upgradei, 1), 0, 0, fnscript<upgradei>},
};
BSDATAF(varianti);