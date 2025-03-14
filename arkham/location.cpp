#include "rand.h"
#include "main.h"

static void select(questa& destination, const quests& source) {
	for(auto& e : source) {
		if(e.index == 1000)
			destination.add(&e);
	}
}

static const quest* find_options(const quests& source) {
	for(auto& e : source) {
		if(e.index == 1001)
			return &e;
	}
	return 0;
}

void locationi::chooseoptions(answers& an) const {
	auto ph = find_options(encounters);
	if(!ph)
		return;
	auto index = ph->index;
	auto pe = bsdata<quest>::end();
	for(auto p = ph + 1; p < pe; p++) {
		if(p->next == -1 || p->index!=index)
			break;
		if(!game.isallow(p->tags))
			continue;
		an.add(p, p->text);
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

static void pick_encounter() {
	player::last->encounter();
}

void locationi::phase_encounter() {
	auto push_header = answers::header;
	answers::header = getnm(id);
	an.clear();
	chooseoptions(an);
	if(encounters)
		an.add(pick_encounter, getnm("PickEncounter"));
	game.applyresult(an.choose());
	answers::header = push_header;
}