#include "nameable.h"
#include "variant.h"

#pragma once

struct consumablei : nameable {
	variants	maximum;
};
struct consumable {
	int			consumables[8];
};