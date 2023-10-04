#include "collection.h"
#include "entity.h"

#pragma once

struct entitya : collection<entity> {
	int			gettotal(ability_s v) const;
};
extern entitya querry;
