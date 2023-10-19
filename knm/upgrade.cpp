#include "player.h"
#include "upgrade.h"

upgradei* last_upgrade;

bool upgradea::isupgrade(const upgradei* v) const {
	auto i = getbsi(v);
	if(i == 0xFFFF)
		return false;
	return (upgrades & (1 << i)) != 0;
}

bool upgradea::isupgradeallow(const upgradei* upgrade) const {
	auto f = upgrade->required;
	return (player->upgrades & f) == f;
}

void upgradea::setupgrade(const upgradei* v) {
	auto i = getbsi(v);
	if(i != 0xFFFF)
		upgrades |= (1 << i);
}