#include "main.h"

static void select(questa& destination, const quests& source) {
	for(auto& e : source) {
		if(e.index == 1000)
			destination.add(&e);
	}
}

const quest* locationi::choose(int count) const {
	questa source; select(source, encounters);
	zshuffle(source.data, source.count);
	if(count < 0)
		return 0;
	else if(count == 1)
		return source.data[0];
	answers an;
	for(auto p : source) {
		if(count-- == 0)
			break;
		an.add(p, p->text);
	}
	return (quest*)an.choose(getnm("ChooseEncounter"));
}