#include "item.h"
#include "script.h"

static void item_add(int type, int count) {
}

template<> void fnscript<itemi>(int value, int bonus) {
	item_add(value, bonus);
}

static void talent_roll(int bonus) {
}

BSDATA(script) = {
	{"TalentRoll", talent_roll}
};
BSDATAF(script)