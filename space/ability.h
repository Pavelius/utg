#include "nameable.h"

#pragma once

enum ability_s : unsigned char {
	Assault, Comunication, Exploration, Medicine, Hunting, Pilot, Research,
	Crew, Discontent, Supply, Machinery,
	Inside, Problem, Success, Advantage,
};
enum statef : unsigned char {
	Ability, NegativeState,
};
struct abilityi : nameable {
	unsigned flags;
	constexpr bool is(statef v) const { return (flags & (1 << v)) != 0; }
};
class abilitya {
	char	abilities[Research + 1];
public:
	void	add(ability_s v, int i) { abilities[v] += i; }
	int		get(ability_s v) const { return abilities[v]; }
	void	set(ability_s v, int i) { abilities[v] = i; }
};