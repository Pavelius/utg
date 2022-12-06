#include "ability.h"
#include "crt.h"

BSDATA(abilityi) = {
	{"Strenght"},
	{"Dexterity"},
	{"Constitution"},
	{"Intellegence"},
	{"Wisdow"},
	{"Charisma"},
	{"SaveVsCharm"},
	{"SaveVsPoison"},
	{"Speed"},
	{"HitPoints"},
	{"ChannelDivinity"},
};
assert_enum(abilityi, ChannelDivinity)