#include "bsdata.h"
#include "module.h"
#include "rand.h"

BSDATA(modulei) = {
	{"ShardCannons", {0, 2}},
	{"ShardCannonsII", {0, 3}},
	{"ShardCannonsIII", {0, 4}},
	{"LaserBeams", {1, 5}},
	{"LaserBeamsII", {1, 7}},
	{"LaserBeamsIII", {1, 10}},
	{"RocketLaunchers", {5, 10}},
	{"RocketLaunchersII", {10, 15}},
	{"RocketLaunchersIII", {15, 25}},
	{"Combat"},
	{"Exploration"},
	{"Damage"},
	{"Effect"},
	{"Bonus"},
	{"Insight"},
	{"Problem"},
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
	{"Alloys"},
	{"Alchogols"},
	{"Weapons"},
	{"Drugs"},
};
assert_enum(modulei, Drugs)

int	critical_count;
modulen last_module;
modulea inflict, suffer;
modulea* last_modules;

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
	critical_count = roll_hits(count, critical);
	return roll_damage(critical_count, minimum * multiplier, maximum * multiplier)
		+ roll_damage(count - critical_count, minimum, maximum);
}

int apply_percent(int value, int percent) {
	return (percent > 0) ? value * 100 / percent : 0;
}

int get_critical(modulen v) {
	switch(v) {
	case ShardCannons: return 5;
	case ShardCannonsII: return 7;
	case ShardCannonsIII: return 10;
	case LaserBeams: return 10;
	case LaserBeamsII: return 15;
	case LaserBeamsIII: return 20;
	case RocketLaunchers: case RocketLaunchersII: case RocketLaunchersIII: return 10;
	default: return 0;
	}
}

int get_critical_multiplayer(modulen v) {
	switch(v) {
	case LaserBeams:
	case LaserBeamsII:
	case LaserBeamsIII:
		return 3;
	default:
		return 2;
	}
}