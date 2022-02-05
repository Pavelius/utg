#include "main.h"

void tilea::select(int from, int to) {
	auto ps = data;
	auto pe = endof();
	for(auto& e : bsdata<tilei>()) {
		if(e.is(Discarded))
			continue;
		if(e.isactive())
			continue;
		if(e.param < from || e.param > to)
			continue;
		*ps++ = &e;
	}
	count = ps - data;
}