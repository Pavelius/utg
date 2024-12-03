#include "ability.h"
#include "collection.h"
#include "class.h"
#include "gender.h"
#include "feat.h"
#include "forcepower.h"
#include "relation.h"
#include "state.h"
#include "wearable.h"

#pragma once

struct creature : wearable, abilityable, classable, skillable, forceable {
	abilityable		basic;
	featf			feats;
	statef			states;
	unsigned short	enemy_id;
	int				experience, hp;
	short			position;
	void			add(classn v);
	void			clear();
	void			damage(int value);
	int				get(abilityn v) const { return abilities[v]; }
	int				get(classn v) const { return classes[v]; }
	int				getbonus(abilityn v) const { return abilities[v] / 2 - 5; }
	creature*		getenemy() const;
	int				getrange(const creature* p) const;
	relation_s		getrelation() const;
	bool			is(staten v) const { return states.is(v); }
	bool			is(relation_s v) const { return getrelation() == v; }
	bool			isenemy(const creature* p) const;
	bool			isfeat(int v) const { return feats.is(v); }
	bool			ismeleefight() const;
	bool			isready() const { return hp > 0; }
	bool			isweararmor() const { return wears[Torso].operator bool(); }
	void			setenemy(const creature* p);
	void			update();
};
extern creature *player, *opponent;
extern collection<creature> creatures, opponents;

void create_hero(int bonus);