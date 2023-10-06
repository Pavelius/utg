#include "ability.h"
#include "entitya.h"

entitya querry, players;

int	entitya::gettotal(ability_s v) const {
	auto result = 0;
	for(auto p : *this)
		result += p->get(v);
	return result;
}

int	entitya::getarmy() const {
	return (gettotal(Army) + 9) / 10;
}