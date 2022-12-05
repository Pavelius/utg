#pragma once
#include "ability.h"
#include "actable.h"
#include "collection.h"
#include "classi.h"
#include "gender.h"
#include "feat.h"

struct creature : actable, abilityable, classable, skillable {
	abilityable	basic;
	featf feats;
	int	money;
	void add(class_s v);
	void clear();
	void create(class_s type, gender_s gender);
	void update();
	void update_ability();
	int	get(ability_s v) const { return abilities[v]; }
	int	get(skill_s v) const { return skills[v]; }
	int	getbonus(ability_s v) const { return abilities[v] / 2 - 5; }
	int	getclass(class_s v) const { return classes[v]; }
	int	gethlevel() const;
	int	getlevel() const;
};
extern creature *player;
extern collection<creature> creatures;