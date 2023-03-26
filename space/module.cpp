#include "crt.h"
#include "module.h"

BSDATA(weapon_typei) = {
	{"Cinetic"},
	{"Beam"},
	{"Rocket"},
};
assert_enum(weapon_typei, Rocket)

int ship::reduce(int& count, ship_stat_s ability) {
	auto v = get(ability);
	if(v > 0) {
		if(count > v) {
			abilities[ability] = 0;
			count -= v;
		} else {
			abilities[ability] -= count;
			count = 0;
		}
	}
	return count;
}

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