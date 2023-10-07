#include "filter.h"
#include "entitya.h"
#include "script.h"

template<> void fnscript<filteri>(int value, int bonus) {
	querry.match(bsdata<filteri>::elements[value].proc, bonus >= 0);
}

template<> bool fntest<filteri>(int value, int bonus) {
	fnscript<filteri>(value, bonus);
	return querry.getcount() != 0;
}