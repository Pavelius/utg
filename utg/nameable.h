#include "crt.h"

#pragma once

struct nameable {
	const char*	id;
	const char*	getid() const { return id; }
	const char*	getname() const { return getnm(id); }
	static const char* getname(const void* object) { return ((nameable*)object)->getname(); }
};
