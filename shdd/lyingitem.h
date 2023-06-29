#include "item.h"
#include "variant.h"

#pragma once

struct lyingitem : item {
	variant		owner;
	static void	add(variant object, item& it);
};
