#include "variant.h"

#pragma once

struct advancement {
	const char*		id;
	variant			type;
	const char*		result;
	const char*		mask;
	variants		conditions;
	variants		elements;
	void			apply(void* object) const;
	void*			choose() const;
};