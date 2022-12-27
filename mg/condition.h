#include "skill.h"

#pragma once

enum condition_s : unsigned char {
	Healthy, HungerAndThirsty, Angry, Tired, Injured, Sick, Dead,
};
typedef flagable<(Dead + 7) / 8> conditiona;
struct conditioni {
	const char*	id;
	skilla		skills;
	skilla		recover;
	char		recover_ob;
};
class conditionable {
	conditiona	conditions;
public:
	bool		is(condition_s v) const { return conditions.is(v); }
};
