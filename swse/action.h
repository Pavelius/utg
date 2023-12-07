#pragma once

#include "feat.h"

enum ability_s : unsigned char;

struct actioni : feati {
	variant			type;
	const char*		getprompt() const;
};
