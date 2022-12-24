#include "condition.h"
#include "property.h"
#include "quest.h"
#include "script.h"
#include "roll12.h"

static void roll(int bonus) {
}

BSDATA(script) = {
	{"Roll", roll},
};
BSDATAF(script)