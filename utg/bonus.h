#include "variant.h"

#pragma once

enum harm_s : unsigned char;

struct bonusi {
	const char*		id;
	harm_s			object;
	int				value, param;
	static variant	getobject(variant v);
	static int		getvalue(variant v);
};
