#include "bsreq.h"
#include "crt.h"
#include "script.h"
#include "strategy.h"
#include "troop.h"
#include "unit.h"

BSDATAC(strategyi, 8)
BSDATAC(troopi, 128 * 8)

BSMETA(abilityi) = {
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
	{"Ability", VAR(abilityi, 1)},
	{"Unit", VAR(uniti, 1)},
};
BSDATAF(varianti);