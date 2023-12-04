#include "groupname.h"
#include "natureable.h"

const char* natureable::getname() const {
	return (name == 0xFFFF) ? getanimal().getname() : bsdata<groupname>::elements[name].name;
}