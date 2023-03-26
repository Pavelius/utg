#include "nameable.h"

#pragma once

enum range_s : unsigned char {
	PrepareStep, LongRange, MediumRange, CloseRange, FinalStep
};
struct rangei : nameable {
};
