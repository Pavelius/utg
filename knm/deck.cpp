#include "card.h"
#include "deck.h"

BSDATA(decki) = {
	{"TacticsDeck"},
	{"LawsDeck"},
	{"ObjectivesDeck"},
	{"SecretObjectivesDeck"},
};
assert_enum(decki, SecretObjectivesDeck)

static void add_cards(entitya& cards, deck_s type) {
	for(auto& e : bsdata<cardi>()) {
		if(!e.usedeck())
			continue;
		if(e.type != type)
			continue;
		for(auto i = 0; i < e.count; i++)
			cards.add(&e);
	}
}

void decki::create() {
	cards.clear();
	add_cards(cards, (deck_s)getbsi(this));
	cards.shuffle();
}