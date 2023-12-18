#include "crt.h"
#include "stringbuilder.h"

#pragma once

struct chooseoption {
	typedef bool(*fntest)(bool run);
	const char*		id;
	fntest			proc;
};
extern chooseoption* last_option;

void choose(const slice<chooseoption>& options, const char* title, ...);
void choosef(const slice<chooseoption>& options);
void chooser(const slice<chooseoption>& options, const char* title, ...);
void choosev(const slice<chooseoption>& options, const char* title, const char* format_param, bool random);
void useopt(const slice<chooseoption>& options, const char* id);

