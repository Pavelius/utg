#include "mission.h"
#include "timeable.h"

#pragma once

struct gamei : timeable, missioni {
	void		clear();
};
extern gamei game;