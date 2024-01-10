#include "ability.h"

BSDATAC(abilityi, 64)

void initialize_abilitites() {
	for(auto& e : bsdata<abilityi>()) {
		auto pn = getnme(stw(e.id, "Format"));
		if(pn)
			e.format = pn;
	}
}