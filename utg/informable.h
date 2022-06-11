#include "stringbuilder.h"

#pragma once

template <typename T>
struct informable {
	static void	getinfo(const void* object, stringbuilder& sb) { ((T*)object)->getinfo(sb); }
};

