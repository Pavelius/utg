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

BSDATAC(landscapei, 32)
BSDATAC(planeti, 512)
BSDATAC(systemi, 32)
BSDATAC(ship, 1024)
BSDATAC(shipi, 256)
BSDATAC(weaponi, 32)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(interval) = {
	BSREQ(min), BSREQ(max),
	{}};
BSMETA(landscapei) = {
	BSREQ(id),
	BSDST(resources, planetri),
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
	{}};
BSMETA(ship) = {
	BSREQ(type),
	BSREQ(position), BSREQ(priority),
	{}};
BSMETA(systemi) = {
	BSREQ(id),
	BSREQ(position),
	{}};
BSMETA(tagi) = {
	BSREQ(id),
	{}};
BSMETA(weapon_typei) = {
	BSREQ(id),
	{}};
BSMETA(weaponi) = {
	BSREQ(id),
	BSREQ(weight), BSREQ(cost),
	BSENM(type, weapon_typei),
	BSREQ(accuracy), BSREQ(damage), BSREQ(rof),
	BSREQ(critical), BSREQ(critical_multiplier),
	{}};
BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VAR(abilityi, 1), 0, 0, fnscript<abilityi>},
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
	{"Weapon", VAR(weaponi, 1)},
};
BSDATAF(varianti)