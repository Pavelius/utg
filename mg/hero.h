#include "actable.h"
#include "condition.h"
#include "crt.h"
#include "enviroment.h"
#include "rang.h"
#include "skill.h"
#include "trait.h"
#include "wise.h"

#pragma once

class creature : public actable, public conditionable, public traitable, public skillable, public wiseable {
};
class hero : public creature {
	unsigned short rang, born;
	skill_s		specialization;
public:
	void		clear();
	void		create();
	const enviromenti* getborn() const { return getbs<enviromenti>(born); }
	const rangi* getrang() const { return getbs<rangi>(rang); }
	void		roll(skill_s skill);
	void		setborn(const enviromenti* p) { born = getbsi(p); }
	void		setrang(const rangi* p) { rang = getbsi(p); }
};
typedef adat<hero*, 8> heroa;
extern heroa party;
extern hero *player, *opponent;