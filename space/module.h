#include "nameable.h"
#include "interval.h"
#include "range.h"

#pragma once

enum modulen : unsigned char {
	ShardCannons, ShardCannonsII, ShardCannonsIII,
	LaserBeams, LaserBeamsII, LaserBeamsIII,
	RocketLaunchers, RocketLaunchersII, RocketLaunchersIII,
	Combat, Exploration,
	Damage, Bonus, Effect, Insight, Problem,
	Crew, Hull, Engine, Sensors, Shield, Armor,
	Foods, Medicaments, Technics, Luxury, Minerals, Alloys, Alchogols, Weapons, Drugs,
	Credits
};
struct modulei : nameable {
	interval damage;
	short	weight, cost;
	bool	isweapon() const { return damage; }
};
struct modulea {
	short	modules[Drugs + 1];
	void	add(modulen v, int i) { modules[v] += i; }
	int		get(modulen v) const { return modules[v]; }
};
extern modulen	last_module;
extern modulea inflict, suffer;
extern modulea*	last_modules;
extern int critical_count;

int apply_percent(int value, int percent);
int roll_damage(int count, int minimum, int maximum);
int roll_damage(int count, int minimum, int maximum, int critical, int multiplier);
int roll_hits(int count, int chance);