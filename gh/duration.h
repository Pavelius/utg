#include "nameable.h"

#pragma once

enum duration_s : unsigned char {
	Instant, Infinite, Round, Use
};
struct durationi : nameable {
};
