#include "bsreq.h"
#include "menu.h"

const menu* menu::last;

BSMETA(menu) = {
	BSREQ(id), BSREQ(source),
	BSREQ(elements),
	{}};
BSDATAC(menu, 128)