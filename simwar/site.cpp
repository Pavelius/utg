#include "site.h"

sitei* lastsite;
site* location;

void site::clear() {
	memset(this, 0, sizeof(*this));
}