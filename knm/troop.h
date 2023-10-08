#include "entity.h"

#pragma once

struct troopi : entity {
	void		paint() const;
};
extern troopi* last_troop;
