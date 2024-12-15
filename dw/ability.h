#pragma once

enum abilityn : unsigned char {
	HP, Damage, Armor, Pierce, Load,
	Strenght, Dexterity, Constitution, Intellegence, Wisdow, Charisma,
	FightingFocus, TravelRole,
};
enum abilitysetn : unsigned char {
	FightingStyles, SpellDomains,
};
struct abilityi {
	const char* id;
};
struct abilityseti {
	const char* id;
};
extern abilityn last_ability;
