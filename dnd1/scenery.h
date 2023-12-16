#pragma once

#include "nameable.h"
#include "typeable.h"

struct sceneryi : nameable {
};
struct scenery : typeable<sceneryi> {
	char		temperature, rainess;
};
extern scenery* scene;
