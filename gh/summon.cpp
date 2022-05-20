#include "main.h"

int summoni::get(action_s v) const {
	switch(v) {
	case Attack: return attack;
	case Move: return move;
	default: return 0;
	}
}

int summoni::get(modifier_s v) const {
	switch(v) {
	case Range: return range;
	default: return 0;
	}
}