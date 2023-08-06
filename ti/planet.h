#include "entity.h"

#pragma once

struct planeti : entity {
	planet_trait_s	trait;
	color_s			speciality;
	char			frame, resources, influence;
	flagable<1>		flags;
	int				get(indicator_s v) const;
	void			paint(unsigned flags) const;
};
extern planeti* last_planet;
