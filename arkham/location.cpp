#include "main.h"

static void select(questa& destination, const quests& source) {
	for(auto& e : source) {
		if(e.index == 1000)
			destination.add(&e);
	}
}

locationi* locationi::find(const char* id) {
	for(auto& e : bsdata<locationi>()) {
		if(equal(e.id, id))
			return &e;
	}
	return 0;
}

const quest* locationi::choose(int count) const {
	questa source; select(source, encounters);
	// zshuffle(source.data, source.count);
	if(count < 0)
		return 0;
	else if(count == 1)
		return source.data[1];
	answers an;
	for(auto p : source)
		an.add(p, p->text);
	return (quest*)an.choose(getnm("ChooseEncounter"));
}