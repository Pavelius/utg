#include "item.h"
#include "script.h"

static void item_add(int type, int count) {
}

template<> void fnscript<itemi>(int value, int bonus) {
	item_add(value, bonus);
}