#include "building.h"

buildingi* lastbuilding;

void building::clear() {
	memset(this, 0, sizeof(*this));
}