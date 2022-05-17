#include "crt.h"

#pragma once

typedef short unsigned cardt;
struct deck : adat<cardt> {
	cardt			choose(const char* title, const array& source, bool need_remove);
	void			discard(cardt v);
	void			shuffle() { zshuffle(data, count); }
	cardt			take();
};