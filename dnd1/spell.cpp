#include "main.h"

BSDATA(spelli) = {
	{"CureLightWound", {1}, Instant, CasterOrCreatureTouched, {1, 6, 1}},
	{"DetectEvil", {1}, Hour, Range120},
	{"DetectMagic", {1, 1}, Turn2, Range60},
	{"Light", {1, 1}, Hour2, Range120},
	{"ProtectionFromEvil", {1, 1}, Hour2, Caster},
	{"PurifyFoodAndWater", {1}, Permanent, Range10},
	{"RemoveFear", {1}, Instant, CasterOrCreatureTouched},
	{"ResistCold", {1}, Hour, Range30},
	{"CharmPerson", {0, 1}, Instant, Range120},
	{"FloatingDisc", {0, 1}, Hour, Range5},
	{"HoldPortal", {0, 1}, Turn2d6, Range10},
	{"MagicMissile", {0, 1}, Instant, Range150, {1, 6, 1}},
	{"ReadLanguages", {0, 1}, Turn2, Caster},
	{"ReadMagic", {0, 1}, Turn, Caster},
	{"Shield", {0, 1}, Turn2, Caster},
	{"Sleep", {0, 1}, Turn4d4, Range240},
	{"Ventriloquism", {0, 1}, Turn2, Range60},
};
assert_enum(spelli, Ventriloquism)