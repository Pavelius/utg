#include "crt.h"
#include "player.h"
#include "province.h"
#include "structure.h"

provincei* province;

void initialize_province() {
	for(auto& e : bsdata<provincei>())
		e.position = {-1000, -1000};
	bsdata<provincei>::elements[0].position = {3, 3};
	bsdata<provincei>::elements[1].position = {2, 3};
	bsdata<provincei>::elements[2].position = {4, 3};
	bsdata<provincei>::elements[3].position = {2, 4};
	bsdata<provincei>::elements[4].position = {3, 4};
	bsdata<playeri>::elements[0].upgrades |= 1|2;
	bsdata<playeri>::elements[0].upgrades |= 2;
}