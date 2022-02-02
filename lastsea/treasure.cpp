#include "main.h"

const treasurei* treasurei::find(const char* id) {
	for(auto& e : bsdata<treasurei>()) {
		if(equal(e.id, id))
			return &e;
	}
	return 0;
}

bool treasurei::isdiscardable() const {
	switch(trigger) {
	case WhenUse: return true;
	case WhenRoll:
		return use.count==0;
	default:
		return false;
	}
}

void treasurei::triggered() {
	switch(trigger) {
	case WhenUse:
		apply();
		break;
	case WhenRoll:
		last_bonus += bonus;
		break;
	}
	if(isdiscardable())
		game.losstreasure(this);
}
