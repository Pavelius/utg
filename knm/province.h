#include "ability.h"
#include "entity.h"

#pragma once

struct provincei : entity, abilitya {
	short		index;
	void		paint() const;
	int			get(ability_s v) const { return abilities[v]; }
};
extern provincei* province;

void initialize_province();