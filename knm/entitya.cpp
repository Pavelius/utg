#include "entitya.h"

entitya querry;

int	entitya::gettotal(ability_s v) const {
	auto result = 0;
	for(auto p : *this)
		result += p->get(v);
	return result;
}