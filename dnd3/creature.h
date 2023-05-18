#include "ability.h"
#include "avatarable.h"
#include "wearable.h"

#pragma once

struct creature : avatarable, statable, wearable {
	void		clear();
	void		create();
};
