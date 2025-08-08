#pragma once

#include "nameable.h"

enum abilityn : unsigned char {
	Speed, Sneak, Fight, Will, Lore, Luck,
	Horror, Combat, Escape, HorrorDamage, CombatDamage, Toughness,
	Health, Sanity, Clue, Money,
	Focus,
};
struct abilityi : nameable {
};