#include "decka.h"
#include "variant.h"

#pragma once

struct actioncard : nameable {
	variant			parent;
	variants		use;
	const char*		trigger;
	char			count;
	bool			isallow() const;
};
extern decka actioncards;
