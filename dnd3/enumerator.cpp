#include "bsreq.h"
#include "crt.h"
#include "enumerator.h"

BSDATAC(enumi, 4096)
BSDATAC(enumgroupi, 256)

BSMETA(enumgroupi) = {
	BSREQ(id),
	{}};
BSMETA(enumi) = {
	BSREQ(id), BSREQ(type),
	{}};

static void auto_increment(enumi* pb, enumi* pe, variant type, int index) {
	while(pb < pe) {
		if(pb->type == type)
			pb->index = index++;
		pb++;
	}
}

void initialize_enumerators() {
	auto pe = bsdata<enumi>::end();
	for(auto& e : bsdata<enumi>()) {
		if(e.index)
			continue;
		auto_increment(&e + 1, pe, e.type, 1);
	}
}