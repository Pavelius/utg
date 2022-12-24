#include "crt.h"
#include "mission.h"

void missioni::clear() {
	memset(this, 0, sizeof(*this));
}

void missioni::beforemove() {
}