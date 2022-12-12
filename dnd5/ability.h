#include "nameable.h"

#pragma once

enum ability_s : unsigned char {
	Strenght, Dexterity, Constitution, Intelligence, Wisdow, Charisma,
	SaveVsCharm, SaveVsPoison, Speed, Proficiency,
	AttackBonus, DamageBonus, ArmorClass, ExtraAttack,
	DivineDomain, FightingStyle,
	HitPoints,
	ActionSurge, ChannelDivinity, Indomitable, SecondWind
};

struct abilityi : nameable {
};