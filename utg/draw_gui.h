#include "datasource.h"
#include "stringbuilder.h"

#pragma once

namespace draw {
struct guii : datasource {
	const char*	id;
	const char*	title;
	const char*	value;
	void*		object;
	int			number, index, column;
	bool		focused, hilighted, checked, disabled;
	unsigned	key, flags;
	fntext		pgetname;
	void		clear();
};
extern guii		gui;
}