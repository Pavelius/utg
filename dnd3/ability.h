#include "nameable.h"
#include "variant.h"

#pragma once

struct abilityi : nameable {
	variants	formula, creating, round;
};
extern variant* script_begin;
extern variant* script_end;