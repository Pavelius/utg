#include "site.h"
#include "slice.h"

const sitei* last_site;
site* location;

void site::clear() {
	memset(this, 0, sizeof(*this));
}