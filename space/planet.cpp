#include "crt.h"
#include "planet.h"
#include "pushvalue.h"

planeti* last_planet;

systemi* planeti::getsystem() const {
	return bsdata<systemi>::elements + system;
}

landscapei* planeti::getlandscape() const {
	return bsdata<landscapei>::elements + landscape;
}

static void update_basic(planetr* dest, planetr* source) {
	memcpy(dest, source, sizeof(*dest));
}

void planeti::update() {
	//pushvalue push(last_planet, this);
	update_basic(this, &bsdata<landscapei>::elements[landscape]);
}