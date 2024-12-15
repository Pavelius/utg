#include "nameable.h"
#include "interval.h"

#pragma once

enum modulen : unsigned char {
	ShardCannons, ShardCannonsII, ShardCannonsIII,
	LaserBeams, LaserBeamsII, LaserBeamsIII,
	RocketLaunchers, RocketLaunchersII, RocketLaunchersIII,
	LightWeapon, HeavyWeapon, Exploration,
	Effect, Insight, Problem,
	Crew, Hull, Engine, Sensors, Shield, Armor,
	Foods, Fuel, Medicaments, Technics, Luxury, Minerals, Alloys, Alchogols, Weapons, Drugs,
	Credits,
};
struct modulei : nameable {
	interval	damage;
	bool		isweapon() const { return damage; }
};
struct modulea {
	short		modules[Credits + 1];
	void		add(modulen v, int i) { modules[v] += i; }
	int			get(modulen v) const { return modules[v]; }
};
extern modulen last_module;
extern int critical_count;

int apply_percent(int value, int percent);
int get_creating_multiplier(modulen v);
int get_critical(modulen v);
int get_critical_multiplayer(modulen v);
int get_hit_chance(modulen v, int distance, int engine);
int roll_damage(int count, int minimum, int maximum);
int roll_damage(int count, int minimum, int maximum, int critical, int multiplier);
int roll_hits(int count, int chance);