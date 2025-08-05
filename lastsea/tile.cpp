#include "main.h"
#include "rand.h"

void tilei::discard() {
	index = 0xFFFF;
	tags.set(Discarded);
}

tilei* tilei::findindex(indext v) {
	for(auto& e : bsdata<tilei>()) {
		if(e.index != v)
			continue;
		if(e.is(Discarded))
			continue;
		return &e;
	}
	return 0;
}

tilei* tilei::pick(int param) {
	for(auto& e : bsdata<tilei>()) {
		if(e.param == param && !e.isactive() && !e.is(Discarded))
			return &e;
	}
	return 0;
}

tilei* tilei::find(int param) {
	for(auto& e : bsdata<tilei>()) {
		if(e.param == param && e.isactive() && !e.is(Discarded))
			return &e;
	}
	return 0;
}

tilei* tilei::pick() {
	tilea source;
	source.select(1, 30);
	if(!source)
		return 0;
	return source.data[rand() % source.count];
}