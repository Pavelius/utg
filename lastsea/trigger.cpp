#include "main.h"

BSDATA(triggeri) = {
	{"NoTrigger"},
	{"WhenRoll"},
	{"WhenThreat"},
	{"WhenAbilityIncreased"},
	{"WhenAbilityDecreased"},
	{"WhenUse"},	
};
assert_enum(triggeri, WhenUse)