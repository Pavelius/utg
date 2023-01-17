#include "crt.h"
#include "range.h"

BSDATA(rangei) = {
	{"Caster"},
	{"CasterOrAlly"},
	{"OneEnemy"},
	{"SomeEnemies"},
	{"AllAlly"},
	{"AllEnemies"},
	{"OneItem"},
	{"OneRandomItem"},
	{"AllCasterItems"},
	{"AllPartyItems"},
	{"Enviroment"},
	{"OneObject"},
	{"OneRandomObject"},
};
assert_enum(rangei, OneRandomObject)

range_s range;