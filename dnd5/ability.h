#include "nameable.h"

#pragma once

enum abilityn : unsigned char {
	Strenght, Dexterity, Constitution, Intelligence, Wisdow, Charisma,
	SaveVsCharm, SaveVsPoison, Speed, Proficiency,
	AttackBonus, DamageBonus, ArmorClass, ExtraAttack, ImprovedCritical, SneakAttack,
	DivineDomain, FightingStyle,
	HitPoints,
	ActionSurge, ChannelDivinity, Indomitable, SecondWind
};

struct abilityi : nameable {
};