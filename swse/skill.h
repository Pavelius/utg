#include "nameable.h"
#include "flagable.h"

#pragma once

typedef flagable<4> skilla;

struct skilli : nameable {
	char		ability;
};
