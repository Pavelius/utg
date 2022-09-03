#include "crt.h"

#pragma once

struct chooseoption {
	typedef bool(*fntest)(bool run);
	const char*		id;
	fntest			proc;
	static void		choose(const slice<chooseoption>& options, const char* title);
};
