#include "bsreq.h"
#include "boardpart.h"
#include "card.h"
#include "crt.h"
#include "list.h"
#include "player.h"
#include "province.h"
#include "script.h"
#include "strategy.h"
#include "structure.h"
#include "tag.h"
#include "troop.h"
#include "unit.h"

BSDATAC(cardi, 512)
BSDATAC(playeri, 32)
BSDATAC(provincei, 128)
BSDATAC(structurei, 8)
BSDATAC(structure, 256)
BSDATAC(strategyi, 8)
BSDATAC(troopi, 128 * 8)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(boardparti) = {
	BSREQ(id),
	{}};
BSMETA(cardi) = {
	BSREQ(id),
	BSREQ(player),
	BSREQ(trigger),
	BSFLG(tags, tagi),
	BSREQ(effect),
	{}};
BSMETA(playeri) = {
	BSREQ(id),
	BSREQ(troops),
	{}};
BSMETA(provincei) = {
	BSREQ(id),
	BSREQ(player),
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
	{"List", VAR(listi, 1), 0, 0, fnscript<listi>, fntest<listi>},
	{"Player", VAR(playeri, 1)},
	{"Province", VAR(provincei, 1)},
	{"Script", VAR(script, 1), 0, 0, fnscript<script>, fntest<script>},
	{"Strategy", VAR(strategyi, 1), ftinfo<strategyi>},
	{"Structure", VAR(structurei, 1), ftinfo<structurei>},
	{"Unit", VAR(uniti, 1), 0, 0, fnscript<uniti>},
};
BSDATAF(varianti);