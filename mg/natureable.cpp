#include "groupname.h"
#include "natureable.h"

const char* natureable::getname() const {
	return (name == 0xFFFF) ? 0 : bsdata<groupname>::elements[name].name;
}