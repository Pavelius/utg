#include "nameable.h"
#include "variant.h"

#pragma once

struct abilityi : nameable {
	variants		formula;
	const char*		format;
};
struct formulai : nameable {
	variants		formula;
	const char*		format;
};