#include "ability.h"
#include "bsreq.h"

BSDATAC(abilityi, 64)
BSDATAC(formulai, 512)

void initialize_abilitites() {
	for(auto& e : bsdata<abilityi>()) {
		auto pn = getnme(stw(e.id, "Format"));
		if(pn)
			e.format = pn;
	}
	for(auto& e : bsdata<formulai>()) {
		auto pn = getnme(stw(e.id, "Format"));
		if(pn)
			e.format = pn;
	}
}