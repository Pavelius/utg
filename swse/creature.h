#pragma once
#include "ability.h"
#include "actable.h"
#include "collection.h"
#include "classi.h"
#include "gender.h"

struct creature : actable, abilityable, classable, skillable {
	abilityable	basic;
	int		money;
	void	add(class_s v);
	void	clear();
	void	create(class_s type, gender_s gender);
	int		get(ability_s v) { return abilities[v]; }
	int		get(skill_s v) { return skills[v]; }
	int		getclass(class_s v) { return classes[v]; }
	int		gethlevel() const;
	int		getbonus(ability_s v) { return abilities[v] / 2 - 5; }
	void	update();
	void	update_ability();
};
extern creature *player;
extern collection<creature> creatures;