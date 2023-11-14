#include "nameable.h"
#include "interval.h"
#include "range.h"

#pragma once

enum module_s : unsigned char {
	ShardCannons, LaserBeams, RocketLaunchers, LightWeapon, HeavyWeapon,
	Exploration,
	Damage, Bonus, Effect, Insight, Problem,
	Crew, Hull, Engine, Sensors, Shield, Armor,
	Foods, Medicaments, Technics, Luxury, Minerals, Alchogols, Weapons, Drugs,
	Credits,
};
struct modulei : nameable {
	short		weight, cost;
};
struct modulea {
	char		modules[Credits + 1];
	void		add(module_s v, int i) { modules[v] += i; }
	int			get(module_s v) const { return modules[v]; }
};
extern module_s	last_module;
extern modulea	inflict, suffer;
extern modulea*	last_modules;

int apply_percent(int value, int percent);
int roll_damage(int count, int minimum, int maximum);
int roll_damage(int count, int minimum, int maximum, int critical, int multiplier);
int roll_hits(int count, int chance);