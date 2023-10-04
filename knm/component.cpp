#include "card.h"
#include "component.h"
#include "crt.h"

BSDATA(componenti) = {
	{"ActionCard"},
	{"CombatCard"},
	{"PlayerAction"},
};
assert_enum(componenti, PlayerAction)