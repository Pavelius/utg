#include "crt.h"
#include "stringbuilder.h"

#pragma once

struct chooseoption {
	typedef bool(*fntest)(bool run);
	const char*		id;
	fntest			proc;
	static void		choosev(const slice<chooseoption>& options, const char* title, const char* format_param, bool random);
	static void		choose(const slice<chooseoption>& options, const char* title, ...) { choosev(options, title, xva_start(title), false); }
	static void		chooser(const slice<chooseoption>& options, const char* title, ...) { choosev(options, title, xva_start(title), true); }
};
