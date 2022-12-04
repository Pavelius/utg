#include "abilityable.h"
#include "crt.h"

void abilityable::clear() {
	memset(this, 0, sizeof(*this));
}