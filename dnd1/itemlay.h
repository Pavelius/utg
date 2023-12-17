#include "item.h"
#include "variant.h"

#pragma once

struct itemlay : item {
	variant		parent;
};
void add_container(variant parent, item& it);