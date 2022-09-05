#include "bsreq.h"
#include "menu.h"

const menu* menu::last;
const char* menu::current_mode = "General";

BSMETA(menu) = {
	BSREQ(id), BSREQ(source),
	BSREQ(elements), BSREQ(mode), BSREQ(format),
	{}};
BSDATAC(menu, 128)

bool menu::isvisible() const {
	if(!mode || !current_mode)
		return true;
	return equal(mode, current_mode);
}