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
	void			add(classn v);
	void			clear();
	int				get(abilityn v) const { return abilities[v]; }
	int				get(classn v) const { return classes[v]; }
	int				getbonus(abilityn v) const { return abilities[v] / 2 - 5; }
	creature*		getenemy() const;
	int				getrange(const creature* p) const { return 0; }
	relation_s		getrelation() const;
	bool			is(staten v) const { return states.is(v); }
	bool			is(relation_s v) const { return getrelation()==v; }
	bool			isenemy(const creature* p) const;
	bool			isfeat(int v) const { return feats.is(v); }
	bool			ismeleefight() const;
	bool			istrain(skill_s v) const { return skills_train.is(v); }
	bool			isweararmor() const { return wears[Torso].operator bool(); }
	void			setenemy(const creature* p);
	void			update();
};
extern creature *player, *opponent;
extern collection<creature> creatures, opponents;

void add_creatures();
void add_item(const char* id);
void create_hero(classn type, gendern gender);