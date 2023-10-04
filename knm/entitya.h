#include "collection.h"
#include "entity.h"

#pragma once

struct provincei;

struct entitya : collection<entity> {
	provincei*	chooseprovince() const;
	int			gettotal(ability_s v) const;
};
extern entitya querry;
