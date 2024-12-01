#pragma once

#include "nameable.h"
#include "variant.h"

enum abilityn : unsigned char;

struct actioni : nameable {
	variant		type;
	variant		upgrade;
	variants	effect;
	const char*	getprompt() const;
};
extern actioni* last_action;