#include "item.h"

int item::getuses() const {
	auto uses = geti().uses;
	return uses ? uses : 1;
}

int	item::getcost() const {
	return geti().coins;
}