#include "crt.h"

#pragma once

struct nameable {
	const char*	id;
	const char*	getname() const { return getnm(id); }
};
