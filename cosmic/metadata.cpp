#include "bsreq.h"
#include "interval.h"
#include "list.h"
#include "questlist.h"
#include "randomizer.h"
#include "script.h"
#include "ship.h"
#include "speech.h"
#include "tag.h"
#include "variant.h"
#include "module.h"

NOBSDATA(interval)

BSDATAC(questlist, 256)
BSDATAC(shipclassi, 16)
BSDATAC(ship, 1024)
BSDATAC(shipi, 256)

BSMETA(interval) = {
	BSREQ(min), BSREQ(max),
	{}};
BSMETA(modulei) = {
	BSREQ(id),
	{}};
BSMETA(script) = {
	BSREQ(id),
	{}};
BSMETA(shipclassi) = {
	BSREQ(id),
	BSREQ(elements),
	{}};
BSMETA(shipi) = {
	BSREQ(id),
	BSENM(type, shipclassi),
	BSREQ(elements),
	{}};
BSMETA(tagi) = {
	BSREQ(id),
	{}};
BSDATA(varianti) = {
	{"NoVariant"},
	{"List", VAR(listi, 1), 0, 0, fnscript<listi>},
	{"Module", VAR(modulei, 1), 0, 0, fnscript<modulei>},
	{"Quest", VAR(questlist, 1), 0, 0, fnscript<questlist>, 0, questlist::read},
	{"RandomList", VAR(randomizeri, 1), 0, 0, fnscript<randomizeri>},
	{"Ship", VAR(shipi, 1), 0, 0, fnscript<shipi>},
	{"ShipClass", VAR(shipclassi, 1)},
	{"Script", VAR(script, 1), 0, 0, fnscript<script>},
	{"Speech", VAR(speech, 1), 0, 0, 0, 0, speech_read},
	{"Tag", VAR(tagi, 1)},
};
BSDATAF(varianti)