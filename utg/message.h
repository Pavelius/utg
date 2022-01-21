#include "variant.h"

#pragma once

struct messagei {
	enum {
		Random, Unique
	};
	typedef variant varianta[4];
	typedef slice<variant> variants;
	variant			type;
	varianta		conditions;
	unsigned		flags;
	const char*		text;
	int				value;
	bool			add(variant v);
	void			clear();
	static const messagei* find(variants source);
	bool			is(int v) const { return ((1 << v) & flags) != 0; }
	bool			match(const messagei::variants& source) const;
	bool			match(variant v) const;
	static void		read(const char* url);
	void			set(int v) { flags |= (v << 1); }
};
