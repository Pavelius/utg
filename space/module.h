#include "nameable.h"
#include "interval.h"
#include "range.h"

#pragma once

enum module_s : unsigned char {
	ShardCannons, ShardCannonsII, ShardCannonsIII,
	LaserBeams, LaserBeamsII, LaserBeamsIII,
	RocketLaunchers, RocketLaunchersII, RocketLaunchersIII,
	LightWeapon, HeavyWeapon, Exploration,
	Damage, Bonus, Effect, Insight, Problem,
	Crew, Hull, Engine, Sensors, Shield, Armor,
	Foods, Medicaments, Technics, Luxury, Minerals, Alloys, Alchogols, Weapons, Drugs,
	Credits,
};
struct modulei : nameable {
	interval	damage;
	short		weight, cost;
	bool		isweapon() const { return damage; }
};
struct modulea {
	short		modules[Credits + 1];
	void		add(module_s v, int i) { modules[v] += i; }
	int			get(module_s v) const { return modules[v]; }
};
extern module_s	last_module;
extern modulea inflict, suffer;
extern modulea*	last_modules;
extern int critical_count;

int apply_percent(int value, int percent);
int roll_damage(int count, int minimum, int maximum);
int roll_damage(int count, int minimum, int maximum, int critical, int multiplier);
int roll_hits(int count, int chance);