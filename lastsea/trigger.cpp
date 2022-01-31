#include "main.h"

BSDATA(triggeri) = {
	{"NoTrigger"},
	{"WhenRoll"},
	{"WhenThreat"},
	{"WhenAbilityIncreased"},
	{"WhenAbilityDecreased"},
	{"WhenDiscard"},
};
assert_enum(triggeri, WhenDiscard)