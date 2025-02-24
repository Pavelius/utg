#include "monster.h"

monsteri* encountered_monster;
monstersa monster_encountered, monster_killed;

interval monsteri::getcount(group_s v) const {
	switch(v) {
	case DungeonLair: case WildernessGroup: return wilderness;
	case WildernessLair: return wilderness.mul(10);
	default: return dungeon;
	}
}

bool monsteri::is(featn i) const {
	for(auto v : feats) {
		if(v.iskind<feati>()) {
			if(v.value == i)
				return true;
		}
	}
	return false;
}

const monsteri*	monsteri::getlook() const {
	if(look)
		return look;
	return this;
}

const char* monsteri::getnature() const {
	if(nature)
		return nature;
	return "Human";
}