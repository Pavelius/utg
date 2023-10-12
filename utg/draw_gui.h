#include "stringbuilder.h"

#pragma once

namespace draw {
struct guii {
	const char*	id;
	const char*	title;
	const char*	value;
	void*		data;
	int			index, number;
	bool		focused, hilighted, checked, disabled;
	unsigned	key, flags;
	fntext		pgetname;
	void		clear();
};
extern guii		gui;
}