#include "ability.h"
#include "collection.h"
#include "classi.h"
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
	short			enemy_id;
	void			add(class_s v);
	void			clear();
	int				get(ability_s v) const { return abilities[v]; }
	int				getbonus(ability_s v) const { return abilities[v] / 2 - 5; }
	int				get(class_s v) const { return classes[v]; }
	creature*		getenemy() const;
	int				getlevel() const;
	int				gethlevel() const;
	int				getrange(const creature* p) const { return 0; }
	relation_s		getrelation() const;
	bool			is(state_s v) const { return states.is(v); }
	bool			is(relation_s v) const { return getrelation()==v; }
	bool			isenemy(const creature* p) const;
	bool			isfeat(int v) const { return feats.is(v); }
	bool			ismeleefight() const;
	bool			istrain(skill_s v) const { return skills_train.is(v); }
	bool			isweararmor() const { return wears[Torso].operator bool(); }
	void			update();
};
extern creature *player, *opponent;
extern collection<creature> creatures, opponents;

void add_creatures();
void add_item(const char* id);
void create_hero(class_s type, gender_s gender);