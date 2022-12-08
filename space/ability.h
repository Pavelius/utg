#pragma once

enum ability_s : unsigned char {
	Assault, Comunication, Exploration, Medicine, Hunting, Research,
};
enum state_s : unsigned char {
	Crew, Discontent, Supply, Machinery,
	Inside, Problem, Success, Advantage,
};
enum statef : unsigned char {
	NegativeState,
};
struct abilityi {
	const char* id;
};
struct statei {
	const char* id;
	unsigned flags;
	constexpr bool is(statef v) const { return (flags & (1 << v)) != 0; }
};
class abilitya {
	char	abilities[Research + 1];
	char	states[Advantage + 1];
public:
	void	add(state_s v, int i) { states[v] += i; }
	int		get(ability_s v) const { return abilities[v]; }
	int		get(state_s v) const { return states[v]; }
	void	set(ability_s v, int i) { abilities[v] = i; }
	void	set(state_s v, int i) { states[v] = i; }
};