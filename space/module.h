#include "nameable.h"
#include "interval.h"
#include "range.h"

#pragma once

enum module_s : unsigned char {
	ShardCannons, LaserBeams, RocketLaunchers,
	Hull, Engine, Sensors, Shield, Armor,
};
struct modulei : nameable {
	short		weight, cost;
};

int apply_percent(int value, int percent);
int roll_damage(int count, int minimum, int maximum);
int roll_damage(int count, int minimum, int maximum, int critical, int multiplier);
int roll_hits(int count, int chance);