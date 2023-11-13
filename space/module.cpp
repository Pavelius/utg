#include "crt.h"
#include "module.h"

BSDATA(modulei) = {
	{"ShardCannons"},
	{"LaserBeams"},
	{"RocketLaunchers"},
	{"LightWeapon"},
	{"HeavyWeapon"},
	{"Damage"},
	{"Effect"},
	{"Bonus"},
	{"Crew"},
	{"Engine"},
	{"Hull"},
	{"Sensors"},
	{"Shield"},
	{"Armor"},
	{"Foods"},
	{"Medicaments"},
	{"Technics"},
	{"Luxury"},
	{"Minerals"},
	{"Alchogols"},
	{"Weapons"},
	{"Drugs"},
	{"Credits"},
};
assert_enum(modulei, Credits)

module_s	last_module;
modulea		inflict, suffer;
modulea*	last_modules;

inline int d100() {
	return rand() % 100;
}

int roll_hits(int count, int chance) {
	auto result = 0;
	for(auto i = 0; i < count; i++) {
		if(d100() < chance)
			result++;
	}
	return result;
}

int roll_damage(int count, int minimum, int maximum) {
	return xrand(count * minimum, count * maximum);
}

int roll_damage(int count, int minimum, int maximum, int critical, int multiplier) {
	auto critical_count = roll_hits(count, critical);
	return roll_damage(critical_count, minimum * multiplier, maximum * multiplier)
		+ roll_damage(count - critical_count, minimum, maximum);
}

int apply_percent(int value, int percent) {
	return (percent > 0) ? value * 100 / percent : 0;
}