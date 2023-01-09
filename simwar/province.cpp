#include "building.h"
#include "province.h"
#include "unit.h"

provincei* province;

int provincei::getbuildings() const {
	auto count = 0;
	for(auto& e : bsdata<building>()) {
		if(e.province == this)
			count++;
	}
	return count;
}

int provincei::getunits() const {
	auto count = 0;
	for(auto& e : bsdata<troop>()) {
		if(e.province == this)
			count++;
	}
	return count;
}