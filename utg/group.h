#include "variant.h"

#pragma once

struct groupi {
	const char*		id;
	static variant	choose(variant type);
};
struct groupvaluei {
	const char*		name;
	variant			type;
	static void		read(const char* url);
};