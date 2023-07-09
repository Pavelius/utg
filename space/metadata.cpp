#include "action.h"
#include "bsreq.h"
#include "game.h"
#include "interval.h"
#include "planet.h"
#include "questlist.h"
#include "list.h"
#include "querry.h"
#include "script.h"
#include "ship.h"
#include "tag.h"
#include "variant.h"
#include "module.h"

NOBSDATA(point)
NOBSDATA(interval)

BSDATAC(actioni, 256)
BSDATAC(landscapei, 32)
BSDATAC(planeti, 512)
BSDATAC(systemi, 32)
BSDATAC(ship, 1024)
BSDATAC(shipi, 256)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(actioni) = {
	BSREQ(id),
	BSENM(state, actionstatei),
	BSENM(back, actionstatei),
	BSREQ(effect), BSREQ(condition),
	BSREQ(cancel),
	{}};
BSMETA(actionstatei) = {
	BSREQ(id),
	{}};
BSMETA(interval) = {
	BSREQ(min), BSREQ(max),
	{}};
BSMETA(landscapei) = {
	BSREQ(id),
	BSDST(resources, planetri),
	BSREQ(resid),
	{}};
BSMETA(modulei) = {
	BSREQ(id),
	{}};
BSMETA(point) = {
	BSREQ(x), BSREQ(y),
	{}};
BSMETA(planeti) = {
	BSREQ(id),
	BSENM(system, systemi),
	BSENM(landscape, landscapei),
	BSREQ(position),
	{}};
BSMETA(planetri) = {
	BSREQ(id),
	{}};
BSMETA(querryi) = {
	BSREQ(id),
	{}};
BSMETA(script) = {
	BSREQ(id),
	{}};
BSMETA(shipi) = {
	BSREQ(id),
	BSENM(kind, shipclassi),
	BSREQ(size),
	{}};
BSMETA(ship) = {
	BSREQ(type),
	BSREQ(position), BSREQ(priority),
	{}};
BSMETA(shipclassi) = {
	BSREQ(id),
	{}};
BSMETA(systemi) = {
	BSREQ(id),
	BSREQ(position),
	{}};
BSMETA(tagi) = {
	BSREQ(id),
	{}};
BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VAR(abilityi, 1), 0, 0, fnscript<abilityi>},
	{"Action", VAR(actioni, 1)},
	{"ActionState", VAR(actionstatei, 1), 0, 0, fnscript<actionstatei>},
	{"Landscape", VAR(landscapei, 1)},
	{"List", VAR(listi, 1), 0, 0, fnscript<listi>},
	{"Planet", VAR(planeti, 1), 0, 0, fnscript<planeti>},
	{"PlanetResource", VAR(planetri, 1)},
	{"Querry", VAR(querryi, 1), 0, 0, fnscript<querryi>},
	{"Quest", VAR(questlist, 1), 0, 0, 0, 0, questlist::read},
	{"Ship", VAR(shipi, 1), 0, 0, fnscript<shipi>},
	{"ShipObject", VAR(ship, 0)},
	{"Script", VAR(script, 1), 0, 0, fnscript<script>},
	{"System", VAR(systemi, 1)},
	{"Tag", VAR(tagi, 1)},
};
BSDATAF(varianti)