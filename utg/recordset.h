#include "crt.h"

#pragma once

struct recordset : public adat<void*> {
	static void* choose(const char* id, const char* title, const char* cancel);
	static void* choose(const array& source, const char* title, const char* cancel);
	void* choose(const char* title, const char* cancel) const;
	void* choosedef(const char* title) const;
	void distinct();
	static const char* getid(const void* v) { return *((const char**)v); }
	void select(const array& source);
	void select(const array& source, fnvisible proc);
	void shuffle();
};