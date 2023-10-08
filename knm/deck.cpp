#include "card.h"
#include "deck.h"

BSDATA(decki) = {
	{"TacticsDeck"},
	{"LawsDeck"},
	{"ObjectivesDeck"},
	{"SecretObjectivesDeck"},
};
assert_enum(decki, SecretObjectivesDeck)

decki* last_deck;

static void add_cards(entitya& cards, const decki* type) {
	for(auto& e : bsdata<cardi>()) {
		if(!e.usedeck())
			continue;
		if(e.location != type)
			continue;
		for(auto i = 0; i < e.count; i++)
			cards.add(&e);
	}
}

void decki::create() {
	cards.clear();
	add_cards(cards, this);
	cards.shuffle();
}