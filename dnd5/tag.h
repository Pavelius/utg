#include "nameable.h"

#pragma once

enum tag_s : unsigned char {
	Blinded,
	Darkvision, Stonecunning, Toughness,
	ImmuneToSleepMagic, HideInEnviroment,
	RitualCasting,
};
struct tagi : nameable {
};
