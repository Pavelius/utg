#include "upgrade.h"

bool upgradea::isupgrade(const upgradei* v) const {
	auto i = getbsi(v);
	if(i == 0xFFFF)
		return false;
	return (upgrades & (1 << i)) != 0;
}

void upgradea::setupgrade(const upgradei* v) {
	auto i = getbsi(v);
	if(i != 0xFFFF)
		upgrades |= (1 << i);
}