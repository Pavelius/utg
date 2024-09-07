#include "bsdata.h"
#include "module.h"
#include "rand.h"

BSDATA(modulei) = {
	{"ShardCannons", {1, 2}},
	{"ShardCannonsII", {1, 3}},
	{"ShardCannonsIII", {1, 4}},
	{"LaserBeams", {1, 5}},
	{"LaserBeamsII", {1, 7}},
	{"LaserBeamsIII", {1, 10}},
	{"RocketLaunchers", {5, 10}},
	{"RocketLaunchersII", {10, 15}},
	{"RocketLaunchersIII", {15, 25}},
	{"LightWeapon"},
	{"HeavyWeapon"},
	{"Exploration"},
	{"Effect"},
	{"Insight"},
	{"Problem"},
	{"Crew"},
	{"Engine"},
	{"Hull"},
	{"Sensors"},
	{"Shield"},
	{"Armor"},
	{"Foods"},
	{"Fuel"},
	{"Medicaments"},
	{"Technics"},
	{"Luxury"},
	{"Minerals"},
	{"Alloys"},
	{"Alchogols"},
	{"Weapons"},
	{"Drugs"},
	{"Credits"},
};
assert_enum(modulei, Credits)

int			critical_count;
module_s	last_module;
modulea		inflict, suffer;
modulea*	last_modules;

int roll_hits(int count, int chance) {
	if(chance <= 0)
		return 0;
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

int get_critical(module_s v) {
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

int get_critical_multiplayer(module_s v) {
	switch(v) {
	case LaserBeams:
	case LaserBeamsII:
	case LaserBeamsIII:
		return 3;
	default:
		return 2;
	}
}

int get_creating_multiplier(module_s v) {
	switch(v) {
	case Hull: return 10;
	default: return 0;
	}
}

int get_hit_chance(module_s v, int distance, int engine) {
	switch(v) {
	case LaserBeams: case LaserBeamsII: case LaserBeamsIII:
		switch(distance) {
		case 3: case 4: return 80 - (engine - 1) * 5;
		default: return 0;
		}
	case RocketLaunchers: case RocketLaunchersII: case RocketLaunchersIII:
		switch(distance) {
		case 2: return 90 - (engine - 1) * 7;
		case 3: return 50 - (engine - 1) * 7;
		default: return 0;
		}
	case ShardCannons: case ShardCannonsII: case ShardCannonsIII:
		switch(distance) {
		case 4: return 90 - (engine - 1) * 5;
		default: return 0;
		}
	default: return 0;
	}
}