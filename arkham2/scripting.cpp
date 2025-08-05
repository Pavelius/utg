#include "bsdata.h"
#include "card.h"
#include "script.h"

static void roll(int bonus) {
}

BSDATA(script) = {
	{"Roll", roll}
};
BSDATAF(script)