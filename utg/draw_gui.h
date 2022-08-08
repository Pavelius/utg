#include "datasource.h"

#pragma once

namespace draw {
struct guii : datasource {
	const char*			id;
	const char*			title;
	const char*			value;
	void*				object;
	int					number;
	bool				checked, disabled;
	unsigned			key;
	unsigned			flags;
	void				clear();
};
extern guii				gui;
}