#include "crt.h"

#pragma once

struct recordset : public adat<void*> {
	void*		choose(const char* title, const char* cancel) const;
	void*		choosedef(const char* title) const;
	void		distinct();
	template<typename T> slice<T*> records() const { return slice<T*>((T**)data, count); }
	void*		random() const;
	void		select(const array& source);
	void		select(const array& source, fnvisible proc);
	void		shuffle();
	static const char* getid(const void* v) { return *((const char**)v); }
	static void* choose(const char* id, const char* title, const char* cancel);
	static void* choose(const array& source, const char* title, const char* cancel);
};