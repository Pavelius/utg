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
	RepairHull, RepairEquipment,
	Research,
};
enum weapon_s : unsigned char {
	Cinetic, Beam, Rocket,
};
typedef char weapona[16];
typedef char defencea[16];
struct weapon_typei : nameable {
};
struct weaponi : itemi {
	weapon_s	type;
	char		accuracy[CloseRange + 1];
	interval	damage; // Minimal and maximum damage
	char		rof; // 3 for cannon, 1 - beam, 2 - rockets
	char		critical, critical_multiplier;
};
struct defencei : itemi {
	weapon_s	type;
	char		armor, shield, deflect, count;
};
struct shipstata {
	char		abilities[Research + 1];
	int			get(ship_stat_s v) const { return abilities[v]; }
	int			reduce(int& count, ship_stat_s ability);
	void		set(ship_stat_s v, int i) { abilities[v] = i; }
};
int apply_percent(int value, int percent);
int roll_damage(int count, int minimum, int maximum);
int roll_damage(int count, int minimum, int maximum, int critical, int multiplier);
int roll_hits(int count, int chance);