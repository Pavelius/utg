#include "actable.h"
#include "condition.h"
#include "crt.h"
#include "rang.h"
#include "skill.h"
#include "wise.h"

#pragma once

class creature : public actable {
	skillds		skills;
	conditiona	conditions;
	wisea		wises;
public:
	void		add(skill_s v, int i) { set(v, get(v) + i); }
	int			get(skill_s v) const { return skills[v]; }
	bool		is(condition_s v) const { return conditions.is(v); }
	bool		is(const wisei* p) const { return p && wises.is(getbsi(p)); }
	void		set(skill_s v, int i) { skills[v] = i; }
	void		set(const wisei* p) { if(p) wises.set(getbsi(p)); }
};
class hero : public creature {
	unsigned short rang;
public:
	void		clear() { memset(this, 0, sizeof(*this)); }
	void		create();
	const rangi* getrang() const { return bsdata<rangi>::elements + rang; }
	void		roll(skill_s skill);
	void		setrang(const rangi* p) { rang = getbsi(p); }
};
typedef adat<hero*, 8> heroa;
extern heroa party;
extern hero *player, *opponent;