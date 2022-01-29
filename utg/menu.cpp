#include "bsreq.h"
#include "menu.h"

int menu::last_index;

BSMETA(menu) = {
	BSREQ(id), BSREQ(source),
	BSREQ(elements),
	{}};
BSDATAC(menu, 128)