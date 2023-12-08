#pragma once

#include "nameable.h"
#include "variant.h"

enum ability_s : unsigned char;

struct actioni : nameable {
	variant			type;
	variant			upgrade;
	variants		effect;
	const char*		getprompt() const;
};
