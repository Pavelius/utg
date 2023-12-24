#include "monster.h"

monsteri* encountered_monster;

interval monsteri::getcount(group_s v) const {
	switch(v) {
	case DungeonLair: case WildernessGroup: return wilderness;
	case WildernessLair: return wilderness.mul(10);
	default: return dungeon;
	}
}