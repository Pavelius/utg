#pragma once

#include "feat.h"

enum ability_s : unsigned char;

struct actioni : feati {
	ability_s		type;
	const char*		getprompt() const;
};
