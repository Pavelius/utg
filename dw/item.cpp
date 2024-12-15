#include "item.h"

int item::getuses() const {
	auto uses_max = geti().uses;
	return uses_max ? uses : 1;
}

int	item::getcost() const {
	return geti().coins;
}