#pragma once

enum ability_s : unsigned char {
	HP, Damage, Armor, Pierce,
	Strenght, Dexterity, Constitution, Intellegence, Wisdow, Charisma,
};
struct abilityi {
	enum { Stat, Indicator };
	const char*		id;
	unsigned		flags;
	constexpr bool	is(unsigned v) const { return (flags & (1 << v)) != 0; }
};
