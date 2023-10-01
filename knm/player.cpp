#include "player.h"

playeri* player;

int	playeri::getmaximum(ability_s v) const {
	switch(v) {
	case Trade: return 3;
	default: return 100;
	}
}