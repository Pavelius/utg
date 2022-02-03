#include "main.h"

BSDATA(triggeri) = {
	{"NoTrigger"},
	{"WhenChooseSpecial"},
	{"WhenRoll"},
	{"WhenThreat"},
	{"WhenAbilityIncreased"},
	{"WhenAbilityDecreased"},
	{"WhenUse"},	
};
assert_enum(triggeri, WhenUse)