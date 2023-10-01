#include "bsreq.h"
#include "crt.h"
#include "filter.h"
#include "list.h"
#include "player.h"
#include "script.h"
#include "strategy.h"
#include "troop.h"
#include "unit.h"

BSDATAC(playeri, 32)
BSDATAC(strategyi, 8)
BSDATAC(troopi, 128 * 8)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(filteri) = {
	BSREQ(id),
	{}};
BSMETA(playeri) = {
	BSREQ(id),
	{}};
BSMETA(strategyi) = {
	BSREQ(id),
	BSREQ(primary), BSREQ(secondary),
	{}};
BSMETA(uniti) = {
	BSREQ(id),
	BSDST(abilities, abilityi),
	BSREQ(combat),
	{}};
BSDATA(varianti) = {
	{"Ability", VAR(abilityi, 1), 0, 0, fnscript<abilityi>},
	{"Filter", VAR(filteri, 1), 0, 0, fnscript<filteri>},
	{"Player", VAR(playeri, 1)},
	{"List", VAR(listi, 1), 0, 0, fnscript<listi>},
	{"Script", VAR(script, 1), 0, 0, fnscript<script>},
	{"Strategy", VAR(strategyi, 1)},
	{"Unit", VAR(uniti, 1), 0, 0, fnscript<uniti>},
};
BSDATAF(varianti);