#include "crt.h"
#include "stringbuilder.h"

#pragma once

struct chooseoption {
	typedef bool(*fntest)(bool run);
	const char*		id;
	fntest			proc;
};
void choosev(const slice<chooseoption>& options, const char* title, const char* format_param, bool random);
void choose(const slice<chooseoption>& options, const char* title, ...);
void chooser(const slice<chooseoption>& options, const char* title, ...);
