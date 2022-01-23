#include "main.h"

piratea::piratea() {
	for(auto i : game.pirates)
		add(bsdata<pirate>::elements + i);
}

piratea::piratea(int action) {
	for(auto i : game.pirates) {
		auto p = bsdata<pirate>::elements + i;
		if(p->isuse(action))
			add(p);
	}
}