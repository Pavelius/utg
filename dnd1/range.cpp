#include "crt.h"
#include "range.h"

BSDATA(rangei) = {
	{"Caster"},
	{"CasterOrAlly"},
	{"OneAlly"},
	{"OneEnemy"},
	{"SomeEnemies"},
	{"AllAlly"},
	{"AllEnemies"},
	{"OneItem"},
	{"OneAllyItem"},
	{"AllCasterItems"},
	{"AllAllyItems"},
	{"Enviroment"},
	{"OneObject"},
	{"OneRandomObject"},
};
assert_enum(rangei, OneRandomObject)

range_s range;