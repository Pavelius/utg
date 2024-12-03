#include "mission.h"
#include "slice.h"

void missioni::clear() {
	memset(this, 0, sizeof(*this));
}

void missioni::beforemove() {
}