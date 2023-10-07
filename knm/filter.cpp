#include "filter.h"
#include "entitya.h"
#include "script.h"

template<> void fnscript<filteri>(int value, int bonus) {
	querry.match(bsdata<filteri>::elements[value].proc, bonus >= 0);
}