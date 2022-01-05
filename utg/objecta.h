#include "crt.h"

#pragma once

struct objecta : public adat<void*> {
	static void* choose(const char* id, const char* title, const char* cancel, bool interactive, const char* resid, const char* header);
	static void* choose(const array& source, const char* title, const char* cancel, bool interactive, const char* resid, const char* header);
	void* choose(const char* title, const char* cancel, bool interactive, const char* resid, const char* header) const;
	void* choosedef(const char* title) const;
	void distinct();
	static const char* getid(const void* v) { return *((const char**)v); }
	void select(const array& source);
	void shuffle();
};