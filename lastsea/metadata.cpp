#include "bsreq.h"
#include "condition.h"
#include "main.h"

#define LNK(V, T) template<> struct bsmeta<V> : bsmeta<T> {};\
template<> struct bsdata<V> : bsdata<T> {};

LNK(abilityn, abilityi)
LNK(trigger_s, triggeri)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(genderi) = {
	BSREQ(id),
	{}};
BSMETA(classi) = {
	BSREQ(id),
	BSREQ(types), BSREQ(levelup), BSREQ(maximum),
	BSREQ(exploration), BSREQ(brawl), BSREQ(hunting), BSREQ(aim), BSREQ(swagger), BSREQ(navigation),
	BSREQ(event1), BSREQ(event2), BSREQ(event3), BSREQ(event4), BSREQ(event5),
	{}};
BSDATAC(classi, 64)
BSMETA(pirate) = {
	{}};
BSDATAC(pirate, 4)
BSMETA(tilei) = {
	BSREQ(param), BSREQ(frame),
	BSFLG(tags, tagi),
	{}};
BSDATAC(tilei, 128)
BSMETA(tagi) = {
	BSREQ(id),
	{}};
BSMETA(quest) = {
	BSREQ(text),
	BSREQ(index), BSREQ(next),
	{}};
BSMETA(triggeri) = {
	BSREQ(id),
	{}};
BSMETA(treasurei) = {
	BSREQ(id),
	BSREQ(abilities),
	BSFLG(tags, tagi),
	BSREQ(use), BSREQ(gain), BSREQ(loss),
	BSREQ(trigger), BSREQ(ability), BSREQ(bonus),
	{}};
BSDATAC(treasurei, 256)
BSMETA(scripti) = {
	BSREQ(id), BSREQ(param),
	{}};

BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VAR(abilityi, 1)},
	{"Card", VAR(treasurei, 1), treasurei::sfgetinfo},
	{"Class", VAR(classi, 1)},
	{"Condition", VAR(conditioni, 1)},
	{"Gender", VAR(genderi, 1)},
	{"Group", VAR(groupi, 1)},
	{"Menu", VAR(menu, 1)},
	{"NavigationTile", VAR(tilei, 0)},
	{"Quest", VAR(quest, 0), gamei::sfgetinfo},
	{"Script", VAR(scripti, 1)},
	{"Tag", VAR(tagi, 1)},
	{"Trigger", VAR(triggeri, 1)},
	{"Value", VAR(groupvaluei, 1)},
	{"Widget", VAR(widget, 1)},
};
assert_enum(varianti, Widget)