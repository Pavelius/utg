#include "nameable.h"
#include "interval.h"
#include "item.h"
#include "range.h"

#pragma once

enum ship_stat_s : unsigned char {
	CineticAccuracy, CineticDamageBonus, CineticArmor,
	CineticEnemyAccuracy, CineticEnemyDamageBonus,
	BeamAccuracy, BeamDamageBonus, BeamArmor,
	BeamEnemyAccuracy, BeamEnemyDamageBonus,
	RocketAccuracy, RocketDamageBonus,
	RocketEnemyAccuracy, RocketEnemyDamageBonus,
};
enum weapon_s : unsigned char {
	Cinetic, Beam, Rocket,
};
typedef char shipsa[RocketEnemyDamageBonus];
typedef char weapona[16];
struct weapon_typei : nameable {
};
struct weaponi : itemi {
	weapon_s	type;
	char		accuracy[FinalStep + 1];
	interval	damage; // Minimal and maximum damage
	char		rof; // 3 for cannon, 1 - beam, 2 - rockets
	char		critical, critical_multiplier;
};
struct ship : itemi {
	weapona		weapons;
	shipsa		abilities;
};