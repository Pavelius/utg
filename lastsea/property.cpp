#include "bsreq.h"
#include "main.h"
#include "widget.h"

#define LNK(V, T) template<> struct bsmeta<V> : bsmeta<T> {};\
template<> struct bsdata<V> : bsdata<T> {};

LNK(ability_s, abilityi)

BSMETA(variant) = {{}};
BSMETA(varianti) = {BSREQ(id), {}};
BSDATAD(variant)

BSMETA(abilityi) = {
	BSREQ(id),
	{}};
BSMETA(genderi) = {
	BSREQ(id),
	{}};
BSMETA(goali) = {
	BSREQ(id),
	BSREQ(danger), BSREQ(reach_location),
	{}};
BSDATAC(goali, 64)
BSMETA(classi) = {
	BSREQ(id),
	BSREQ(types), BSREQ(levelup), BSREQ(maximum),
	BSREQ(exploration), BSREQ(brawl), BSREQ(hunting), BSREQ(aim), BSREQ(swagger), BSREQ(navigation),
	{}};
BSDATAC(classi, 64)
BSMETA(pirate) = {
	{}};
BSDATAC(pirate, 4)
BSMETA(speciali) = {
	BSREQ(id),
	{}};
BSMETA(tilei) = {
	BSREQ(page), BSREQ(frame),
	{}};
BSDATAC(tilei, 128)
BSMETA(tagi) = {
	BSREQ(id),
	{}};
BSMETA(widget) = {
	BSREQ(id),
	{}};
BSMETA(quest) = {
	BSREQ(text), BSREQ(image), BSREQ(header),
	BSREQ(index), BSREQ(next),
	{}};
BSMETA(treasurei) = {
	BSREQ(id),
	BSREQ(abilities),
	BSFLG(tags, tagi),
	BSREQ(use), BSREQ(gain), BSREQ(loss),
	{}};
BSDATAC(treasurei, 256)

#define VRSTD(T) bsmeta<T>::meta, bsdata<T>::source_ptr
BSDATA(varianti) = {
	{"NoVariant"},
	{"Ability", VRSTD(abilityi), 1},
	{"Card", VRSTD(treasurei), 1, 0, treasurei::sfgetinfo},
	{"Class", VRSTD(classi), 1},
	{"Gender", VRSTD(genderi), 1},
	{"Goal", VRSTD(goali), 1},
	{"Group", VRSTD(groupi), 1},
	{"Menu", VRSTD(menu), 1},
	{"NavigationTile", VRSTD(tilei), 2},
	{"Quest", VRSTD(quest), 0, 0, gamei::sfgetinfo},
	{"Special", VRSTD(speciali), 1},
	{"Tag", VRSTD(tagi), 1},
	{"Value", VRSTD(groupvaluei), 1},
	{"Widget", VRSTD(widget), 1},
};
assert_enum(varianti, Widget)