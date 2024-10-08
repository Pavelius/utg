#include "avatarable.h"
#include "condition.h"
#include "enviroment.h"
#include "natureable.h"
#include "rang.h"
#include "skill.h"
#include "trait.h"
#include "wise.h"

#pragma once

class creature : public natureable, public conditionable, public traitable, public skillable, public wiseable {
};
class hero : public creature, public avatarable {
	unsigned short	rang, born;
	skill_s			specialization;
public:
	void			clear();
	void			roll(skill_s skill, skill_s opponent_skill, creature* opponent);
	void			create();
	const enviromenti* getborn() const { return getbs<enviromenti>(born); }
	void			getinfo(stringbuilder& sb) const;
	const rangi*	getrang() const { return getbs<rangi>(rang); }
	static void		getproperty(const void* object, variant v, stringbuilder& sb);
	void			roll(skill_s skill);
	void			setborn(const enviromenti* p) { born = getbsi(p); }
	void			setrang(const rangi* p) { rang = getbsi(p); }
};
typedef adat<hero*, 8> heroa;
extern heroa		party;
extern hero			*player;
extern creature		*opponent;