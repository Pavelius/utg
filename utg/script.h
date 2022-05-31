#include "variant.h"

#pragma once

struct script {
	typedef void(*fnscript)(int bonus, int param);
	typedef void(*fnapply)(variant v);
	const char*		id;
	fnscript		proc;
	int				param;
	static fnapply	custom;
	static fnapply	foreach;
	static bool		stop;
	static void		setforeach(int bonus, int param);
	static void		run(variant v);
	static void		run(variants source);
};
