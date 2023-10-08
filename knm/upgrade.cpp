#include "upgrade.h"

bool upgradea::isupgrade(const upgradei* v) const {
	auto i = getbsi(v);
	if(i == 0xFFFF)
		return false;
	return (upgrades & (1 << i)) != 0;
}