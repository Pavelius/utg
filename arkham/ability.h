#pragma once

enum abilityn : unsigned char {
	Speed, Sneak, Fight, Will, Lore, Luck,
	Horror, Combat, Escape, HorrorDamage, CombatDamage, Toughness,
	Health, Sanity, Clue, Money,
	Focus,
};
struct abilityi {
	enum { Stat, Indicator };
	const char*		id;
	unsigned		flags;
	constexpr bool	is(unsigned v) const { return (flags & (1 << v)) != 0; }
};
