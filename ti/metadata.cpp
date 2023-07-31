#include "bsreq.h"
#include "condition.h"
#include "list.h"
#include "objective.h"
#include "main.h"
#include "variant.h"

template<> void ftstatus<script>(const void* object, stringbuilder& sb);

BSDATAC(actioncard, 128)
BSDATAC(entity, 512)
BSDATAC(objectivei, 64)
BSDATAC(planeti, 64)
BSDATAC(playeri, 32)
BSDATAC(strategyi, 8)
BSDATAC(systemi, 64)
BSDATAC(troop, 256)
BSDATAC(uniti, 32)
BSDATAC(unitupgrade, 32)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(actioncard) = {
	BSREQ(id),
	BSREQ(parent),
	BSREQ(count),
	BSREQ(trigger),
	BSREQ(use),
	{}};
BSMETA(colori) = {
	BSREQ(id),
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
	BSREQ(player),
	{}};
BSMETA(tilei) = {
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
	{"NoVariant"},
	{"ActionCard", VAR(actioncard, 1)},
	{"Condition", VAR(conditioni, 1)},
	{"Indicator", VAR(indicatori, 1), ftstatus<indicatori>, 0, fnscript<indicatori>, fntest<indicatori>},
	{"List", VAR(listi, 1), 0, 0, fnscript<listi>, fntest<listi>},
	{"Objective", VAR(objectivei, 1)},
	{"Planet", VAR(planeti, 1)},
	{"Player", VAR(playeri, 1), ftstatus<playeri>},
	{"Script", VAR(script, 1), ftstatus<script>, 0, fnscript<script>, fntest<script>},
	{"Strategy", VAR(strategyi, 1), ftstatus<strategyi>},
	{"System", VAR(systemi, 1)},
	{"Tech", VAR(techi, 1), ftstatus<techi>},
	{"Unit", VAR(uniti, 1), 0, 0, fnscript<uniti>},
	{"UnitType", VAR(unit_typei, 1)},
	{"UnitUpgrade", VAR(unitupgrade, 1)},
};
BSDATAF(varianti)