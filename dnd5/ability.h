#include "nameable.h"

#pragma once

enum ability_s : unsigned char {
	Strenght, Dexterity, Constitution, Intelligence, Wisdow, Charisma,
	SaveVsCharm, SaveVsPoison, Speed, HitPoints, ChannelDivinity,
};

struct abilityi : nameable {
};