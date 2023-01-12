#include "site.h"

sitei* lastsite;

void site::clear() {
	memset(this, 0, sizeof(*this));
}