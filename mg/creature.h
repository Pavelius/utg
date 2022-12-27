#include "actable.h"
#include "condition.h"
#include "crt.h"
#include "skill.h"

#pragma once

class creature : public actable {
	skillds		skills;
	conditiona	conditions;
public:
	int			get(skill_s v) const { return skills[v]; }
	bool		is(condition_s v) const { return conditions.is(v); }
	void		set(skill_s v, int i) { skills[v] = i; }
};
class hero : public creature {
public:
	void		roll(skill_s skill);
};
typedef adat<hero*, 8> creaturea;
extern creaturea party, helpers;