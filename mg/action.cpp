#include "action.h"
#include "bsdata.h"

BSDATA(actioni) {
	{"Attack", {IndependedRoll, VersusRoll, IndependedRoll, VersusRoll}},
	{"Defend", {VersusRoll, IndependedRoll, NoRoll, VersusRoll}},
	{"Feint", {NoRoll, IndependedRoll, VersusRoll, IndependedRoll}},
	{"Maneuver", {VersusRoll, VersusRoll, IndependedRoll, IndependedRoll}},
};
assert_enum(actioni, Maneuver)