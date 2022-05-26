#include "crt.h"

#pragma once

struct strvar {
	const char*		data;
	size_t			count;
	constexpr strvar() : data(0), count(0) {}
	constexpr strvar(const char* v) : data(v), count(zlen(v)) {}
};
