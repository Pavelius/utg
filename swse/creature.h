#include "abilityable.h"
#include "actable.h"
#include "collection.h"
#include "classi.h"
#include "gender.h"

#pragma once

struct creature : actable, abilityable {
	abilityable	basic;
	char		classes[16];
	int			money;
	void		addclass(const classi* pv);
	void		clear();
	void		create(const classi* pclass, gender_s gender);
};
extern creature *player;
extern collection<creature> creatures;