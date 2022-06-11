#include "main.h"

BSDATA(techi) {
	{"PlasmaScoring", Red, {}},
	{"MagenDefenseGrid", Red, {1}},
	{"DuraniumArmor", Red, {2}},
	{"AssaultCannon", Red, {3}},
	{"NeuralMotivator", Green, {}},
	{"DacxiveAnimators", Green, {0, 1}},
	{"HyperMetabolism", Green, {0, 2}},
	{"X89BacterialWeapon", Green, {0, 3}},
	{"AntimassDeflectors", Blue, {}},
	{"GravityDriver", Blue, {0, 0, 1}},
	{"FleetLogistics", Blue, {0, 0, 2}},
	{"LightWaveDeflector", Blue, {0, 0, 3}},
	{"SarweenTools", Yellow, {}},
	{"GravitonLaserSystem", Yellow, {0, 0, 0, 1}},
	{"TransitDiodes", Yellow, {0, 0, 0, 2}},
	{"IntegratedEconomy", Yellow, {0, 0, 0, 3}},
};
assert_enum(techi, IntegratedEconomy)

bool techi::match(const requirement& e) const {
	for(auto i = 0; i < 4; i++) {
		if(required[i] > e.required[i])
			return false;
	}
	return true;
}