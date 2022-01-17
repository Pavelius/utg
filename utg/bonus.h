#include "variant.h"

#pragma once

struct bonusi {
	const char*		id;
	variant			object;
	int				value, param;
	static variant_s this_type;
	static variant	getobject(variant v);
	static int		getvalue(variant v);
};
