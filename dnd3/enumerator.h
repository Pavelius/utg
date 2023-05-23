#include "nameable.h"
#include "variant.h"

#pragma once

struct enumi : nameable {
	variant			type;
	int				index;
	static void		initialize();
};