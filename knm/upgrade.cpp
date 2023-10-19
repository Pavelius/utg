#include "player.h"
#include "upgrade.h"

upgradei* last_upgrade;
const int max_upgrade = 31;

int	upgradea::getupgradecount() const {
	int result = 0;
	for(auto i = 0; i <= max_upgrade; i++) {
		if((upgrades & (1 << i)) != 0)
			result++;
	}
	return result;
}

bool upgradea::isupgrade(const upgradei* v) const {
	auto i = getbsi(v);
	if(i == 0xFFFF)
		return false;
	return isupgrade(i);
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

int	upgradea::getupgrade(ability_s v) const {
	auto result = 0;
	for(auto i = 0; i <= max_upgrade; i++) {
		if(isupgrade(i))
			result += bsdata<upgradei>::elements[i].abilities[v];
	}
	return result;
}