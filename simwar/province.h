#include "bsref.h"
#include "crt.h"
#include "landscape.h"

#pragma once

struct provincei : nameable {
	bsref<landscapei>	landscape;
};
