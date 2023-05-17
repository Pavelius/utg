#include "ability.h"
#include "wearable.h"

#pragma once

struct creature : statable, wearable {
	void		create();
	void		clear();
};
