#include "main.h"

static bool if_element(int bonus, int param) {
	return false;
}

void initialize_conditions() {
	conditioni::add("IfAir", if_element, Air);
	conditioni::add("IfFire", if_element, Fire);
	conditioni::add("IfEarth", if_element, Earth);
	conditioni::add("IfIce", if_element, Ice);
	conditioni::add("IfLight", if_element, Light);
	conditioni::add("IfDark", if_element, Dark);
}

static void opponent(int bonus, int param) {
}

BSDATA(scripti) = {
	{"Opponent", opponent},
};
BSDATAF(scripti)