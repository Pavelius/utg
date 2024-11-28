#include "ability.h"
#include "bsreq.h"
#include "stringbuilder.h"

BSDATAC(abilityi, 64)
BSDATAC(formulai, 512)

void initialize_abilitites() {
	for(auto& e : bsdata<abilityi>()) {
		auto pn = getnme(ids(e.id, "Format"));
		if(pn)
			e.format = pn;
	}
	for(auto& e : bsdata<formulai>()) {
		auto pn = getnme(ids(e.id, "Format"));
		if(pn)
			e.format = pn;
	}
}