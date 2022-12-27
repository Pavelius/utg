#include "actable.h"
#include "condition.h"
#include "crt.h"
#include "skill.h"

#pragma once

class creature : public actable {
	skillds		skills;
	conditiona	conditions;
public:
	void		add(skill_s v, int i) { set(v, get(v) + i); }
	int			get(skill_s v) const { return skills[v]; }
	bool		is(condition_s v) const { return conditions.is(v); }
	void		set(skill_s v, int i) { skills[v] = i; }
};
class hero : public creature {
public:
	void		clear() { memset(this, 0, sizeof(*this)); }
	void		create();
	void		roll(skill_s skill);
};
typedef adat<hero*, 8> heroa;
extern heroa party;
extern hero *player, *opponent;