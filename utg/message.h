#include "variant.h"

#pragma once

struct messagei {
	typedef variant varianta[4];
	typedef slice<variant> variants;
	varianta		conditions;
	const char*		text;
	int				value;
	bool			add(variant v);
	static const messagei* find(variants source);
	bool			match(const messagei::variants& source) const;
	bool			match(variant v) const;
	static void		read(const char* url);
};
