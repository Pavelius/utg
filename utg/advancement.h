#include "answers.h"
#include "variant.h"

#pragma once

struct advancement {
	typedef void fnadd(answers& source, const void* object, variant v);
	typedef bool fnset(void* object, const char* id, const void* value, int count);
	const char*	id;
	variant		type;
	const char*	result;
	char		choose;
	variants	elements;
	void		apply(void* object, fnadd padd = 0, fnset pset = 0) const;
	void*		choosevalue(const void* object, fnadd padd = 0) const;
};