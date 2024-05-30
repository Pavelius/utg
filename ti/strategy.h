#include "entity.h"

#pragma once

struct strategyi : entity {
	int			initiative;
	variants	primary, secondary;
};
extern strategyi* last_strategy;