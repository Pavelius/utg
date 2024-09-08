#include "ability.h"
#include "adat.h"
#include "entity.h"
#include "point.h"

#pragma once

struct provincei : entity, abilitya {
	point			position;
	adat<point,8>	points;
	void			paint() const;
	int				get(ability_s v) const { return abilities[v]; }
	void			setposition(point v);
};
extern provincei* province;

void initialize_province();