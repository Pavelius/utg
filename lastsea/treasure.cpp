#include "main.h"

const treasurei* treasurei::find(const char* id) {
	for(auto& e : bsdata<treasurei>()) {
		if(equal(e.id, id))
			return &e;
	}
	return 0;
}

bool treasurei::isdiscardable() const {
	return tags.is(Discard);
}

void treasurei::triggered() {
	switch(trigger) {
	case WhenUse:
	case WhenChooseSpecial:
		apply();
		break;
	case WhenRoll:
		last_bonus += bonus;
		break;
	}
	if(isdiscardable())
		game.losstreasure(this);
}
