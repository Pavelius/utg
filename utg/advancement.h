#include "answers.h"
#include "variant.h"

#pragma once

struct advancement {
	typedef void fnadd(answers& source, const void* object, variant v);
	typedef bool fnset(void* object, const char* id, const void* value);
	const char*		id;
	variant			type;
	const char*		result;
	const char*		mask;
	variants		conditions;
	variants		elements;
	void			apply(void* object, fnadd padd = 0, fnset pset = 0) const;
	void*			choose(const void* object, fnadd padd = 0) const;
};