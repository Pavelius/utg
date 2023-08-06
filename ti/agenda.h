#include "entity.h"
#include "variant.h"

#pragma once

struct agendai : entity {
	variants		target;
	variants		yes, no;
};