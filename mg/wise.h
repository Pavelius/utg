#include "flagable.h"
#include "nameable.h"
#include "variant.h"

#pragma once

struct wisei : nameable {
	variant		subject;
};
typedef flagable<16> wisea;