#include "ability.h"
#include "collection.h"
#include "classi.h"
#include "gender.h"
#include "feat.h"
#include "forcepower.h"
#include "state.h"
#include "wearable.h"

#pragma once

struct creature : wearable, abilityable, classable, skillable, forceable {
	abilityable		basic;
	featf			feats;
	statef			states;
	void			add(class_s v);
	void			clear();
	void			create(class_s type, gender_s gender);
	int				get(ability_s v) const { return abilities[v]; }
	int				getbonus(ability_s v) const { return abilities[v] / 2 - 5; }
	int				get(class_s v) const { return classes[v]; }
	int				gethlevel() const;
	int				getlevel() const;
	bool			is(state_s v) const { return states.is(v); }
	bool			istrain(skill_s v) const { return skills_train.is(v); }
	bool			isweararmor() const { return wears[Torso].operator bool(); }
	void			update();
};
extern creature *player;
extern collection<creature> creatures;