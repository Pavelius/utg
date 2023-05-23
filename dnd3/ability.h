#include "nameable.h"
#include "variant.h"

#pragma once

struct abilityi : nameable {
	variants	formula, round;
};
struct challengei {
	variant		roll;
	char		dc;
};