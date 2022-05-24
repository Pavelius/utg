#include "main.h"

BSDATA(triggeri) = {
	{"NoTrigger"},
	{"WhenChooseSpecial"},
	{"WhenRoll"},
	{"WhenRollSuccess"},
	{"WhenThreat"},
	{"WhenAbilityIncreased"},
	{"WhenAbilityDecreased"},
	{"WhenUse"},	
};
assert_enum(triggeri, WhenUse)