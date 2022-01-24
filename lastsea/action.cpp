#include "main.h"

int actioni::getstage(int v) const {
	if(v < 1)
		v = 1;
	auto r = 0;
	for(auto minimum : result) {
		if(!minimum)
			break;
		if(v >= minimum)
			r++;
	}
	return r;
}

variants actioni::getoutcome(int v) const {
	switch(v) {
	case 1: return outcome1;
	case 2: return outcome2;
	case 3: return outcome3;
	case 4: return outcome4;
	case 5: return outcome5;
	case 6: return outcome6;
	default: return {};
	}
}

void actioni::choose(int count) const {
	struct actionchoose : utg::choosei {
		actionchoose(answers& an) : choosei(an) {}
		void apply(int index, const void* object) {
			auto p = (casei*)object;
			game.apply(p->outcome);
		}
	};
	answers	an;
	actionchoose source(an);
	variant parent = this;
	for(auto& e : bsdata<casei>()) {
		if(e.type != parent)
			continue;
		an.add(&e, getnm(e.id));
	}
	source.choose(0, count);
}

bool actioni::is(special_s v) const {
	for(auto e : script) {
		if(e.type == Special && e.value == v)
			return true;
	}
	return false;
}