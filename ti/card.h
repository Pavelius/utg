#include "decka.h"
#include "entity.h"
#include "variant.h"

#pragma once

struct card : entity {
	variants		use;
	const char*		trigger;
	char			count;
	bool			isallow() const;
};
extern decka actioncards;
