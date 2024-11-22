#include "bsreq.h"
#include "list.h"
#include "script.h"

BSMETA(listi) = {
	BSREQ(id),
	BSREQ(elements),
	{}};
BSDATAC(listi, 256)

void listi::run() const {
	script_run(id, elements);
}

void run_command(const char* id) {
	auto p = bsdata<listi>::find(id);
	if(p)
		p->run();
}