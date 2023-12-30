#include "collection.h"

#pragma once

struct statistici {
	void*	object;
	int		count;
};
struct statistica : collection<statistici> {
	void		add(void* object, int count);
	statistici*	findadd(void* object);
};
