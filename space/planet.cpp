#include "crt.h"
#include "planet.h"

planeti* current_planet;

systemi* planeti::getsystem() const {
	return bsdata<systemi>::elements + system;
}

landscapei* planeti::getlandscape() const {
	return bsdata<landscapei>::elements + landscape;
}