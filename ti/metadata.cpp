#include "bsreq.h"
#include "condition.h"
#include "list.h"
#include "main.h"
#include "variant.h"

#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr

BSDATAC(actioncard, 128)
BSDATAC(card, 256)
BSDATAC(component, 128)
BSDATAC(objectivei, 64)
BSDATAC(planeti, 64)
BSDATAC(playeri, 32)
BSDATAC(strategyi, 8)
BSDATAC(systemi, 64)
BSDATAC(troop, 256)
BSDATAC(uniti, 32)
BSDATAC(unitupgrade, 32)
BSDATAD(variant)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(actioncard) = {
	BSREQ(id),
	BSREQ(count),
	BSREQ(trigger),
	BSREQ(use),
	{}};
BSMETA(choosestep) = {
	BSREQ(id),
	{}};
BSMETA(colori) = {
	BSREQ(id),
	{}};
BSMETA(component) = {
	BSREQ(id),
	BSREQ(parent),
	BSREQ(trigger),
	BSREQ(use),
	{}};
BSMETA(indicatori) = {
	BSREQ(id),
	{}};
BSMETA(objectivei) = {
	BSREQ(id),
	BSREQ(condition), BSREQ(value), BSREQ(stage),
	{}};
BSMETA(planeti) = {
	BSREQ(id),
	BSENM(trait, planet_traiti),
	BSENM(speciality, colori),
	BSREQ(resources), BSREQ(influence),
	BSREQ(frame),
	BSREF(location, systemi),
	{}};
BSMETA(planet_traiti) = {
	BSREQ(id),
	{}};
BSMETA(playeri) = {
	BSREQ(id),
	BSREQ(commodities),
	BSREQ(troops),
	BSREQ(startup),
	BSFLG(race, racefi),
	{}};
BSMETA(racefi) = {
	BSREQ(id),
	{}};
BSMETA(script) = {
	BSREQ(id),
	{}};
BSMETA(strategyi) = {
	BSREQ(id),
	BSREQ(initiative),
	BSREQ(primary), BSREQ(secondary),
	{}};
BSMETA(systemi) = {
	BSREQ(id),
	BSREQ(home),
	BSREQ(special_index),
	BSENM(special, tilei),
	{}};
BSMETA(tagi) = {
	BSREQ(id),
	{}};
BSMETA(techi) = {
	BSREQ(id),
	BSENM(color, colori),
	{}};
BSMETA(tilei) = {
	BSREQ(id),
	{}};
BSMETA(triggeri) = {
	BSREQ(id),
	{}};
BSMETA(uniti) = {
	BSREQ(id),
	BSENM(type, unit_typei),
	BSDST(abilities, abilityi),
	BSFLG(tags, tagi),
	BSREQ(race),
	BSREQ(replace),
	{}};
BSMETA(unit_typei) = {
	BSREQ(id),
	{}};
BSMETA(unitupgrade) = {
	BSREQ(id),
	BSENM(type, unit_typei),
	BSDST(abilities, abilityi),
	BSFLG(tags, tagi),
	BSREQ(race),
	BSREQ(replace),
	BSREQ(required),
	{}};

BSDATA(varianti) = {
	{"ActionCard", VRSTD(actioncard), 1},
	{"Component", VRSTD(component), 1},
	{"Condition", VRSTD(conditioni), 1},
	{"Indicator", VRSTD(indicatori), 1},
	{"List", VRSTD(listi), 1},
	{"Objective", VRSTD(objectivei), 1},
	{"Planet", VRSTD(planeti), 1},
	{"Player", VRSTD(playeri), 1},
	{"Script", VRSTD(script), 1},
	{"Step", VRSTD(choosestep), 1},
	{"Strategy", VRSTD(strategyi), 1},
	{"System", VRSTD(systemi), 1},
	{"Tech", VRSTD(techi), 1},
	{"Unit", VRSTD(uniti), 1},
	{"UnitType", VRSTD(unit_typei), 1},
	{"UnitUpgrade", VRSTD(unitupgrade), 1},
};
BSDATAF(varianti)