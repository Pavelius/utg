#include "area.h"

area* placeable::getplace() const {
	return getbs<area>(place);
}

void placeable::setplace(const area* v) {
	place = getbsi(v);
}