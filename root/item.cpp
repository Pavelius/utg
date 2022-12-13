#include "item.h"

void item::create(unsigned char item_type) {
	type = item_type;
	auto& ei = geti();
	wear_max = ei.wear;
	wear = wear_max;
}