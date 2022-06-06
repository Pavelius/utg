#include "main.h"

static bool if_element(int bonus, int param) {
	return false;
}

static bool if_adjanced_ally(int bonus, int param) {
	return false;
}

static bool if_not_adjanced_his_ally(int bonus, int param) {
	return false;
}

static bool if_state(int bonus, int param) {
	return false;
}

void initialize_conditions() {
	conditioni::add("IfAir", if_element, Air);
	conditioni::add("IfDark", if_element, Dark);
	conditioni::add("IfEarth", if_element, Earth);
	conditioni::add("IfFire", if_element, Fire);
	conditioni::add("IfIce", if_element, Ice);
	conditioni::add("IfInvisibility", if_state, Invisibility);
	conditioni::add("IfLight", if_element, Light);
	conditioni::add("IfTargetAdjancedAlly", if_adjanced_ally);
	conditioni::add("IfTargetNotAdjancedHisAlly", if_not_adjanced_his_ally);
}

static void opponent(int bonus, int param) {
}

static void double_attack(int bonus, int param) {
}

static void suffer_damage(int bonus, int param) {
}

static void recover_discarded(int bonus, int param) {
}

BSDATA(scripti) = {
	{"AttackDouble", double_attack},	
	{"SufferDamage", suffer_damage},
	{"RecoverDiscarded", recover_discarded},
};
BSDATAF(scripti)