#include "nameable.h"
#include "interval.h"
#include "item.h"
#include "range.h"

#pragma once

enum module_s : unsigned char {
	ShardCannons, LaserBeams, RocketLaunchers,
	Hull, Engine, Sensors, Shield, Armor,
};
struct modulea {
	char		modules[Armor + 1];
	int			get(module_s v) const { return modules[v]; }
	int			reduce(int& count, module_s ability);
	void		set(module_s v, int i) { modules[v] = i; }
};
struct modulei : nameable {
	short		weight, cost;
	char		power;
};

int apply_percent(int value, int percent);
int roll_damage(int count, int minimum, int maximum);
int roll_damage(int count, int minimum, int maximum, int critical, int multiplier);
int roll_hits(int count, int chance);