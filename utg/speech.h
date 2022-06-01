#include "variant.h"

#pragma once

typedef adat<struct speech*, 32> speecha;

struct speech {
	variant			keys[3];
	const char*		name;
	void			clear() { memset(this, 0, sizeof(*this)); }
	static speech*	find(const slice<variant>& key);
	static bool		match(variant key, const variants& values);
	static bool		match(const slice<variant>& key, const variants& values);
	static void		read(const char* url);
	static void		readl(const char* url);
};