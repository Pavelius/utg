#include "collection.h"
#include "flagable.h"
#include "nameable.h"
#include "statable.h"

#pragma once

typedef flagable<8> tacticaf;
struct tactici : nameable {
	costac			effect, cost, perunit;
	tacticaf		disable;
	const tactici*	upgrade;
	bool			isdisable(const tactici* v) const;
};
struct tactica : collection<tactici> {
	void			add(const tactici* v);
	void			add(const tacticaf& v);
};