#include "card.h"
#include "deck.h"

BSDATA(decki) = {
	{"LawsDeck"},
	{"ObjectivesDeck"},
	{"SecretObjectivesDeck"},
	{"TacticsDeck"},
	{"UpgradesDeck"},
};
assert_enum(decki, UpgradesDeck)

decki* last_deck;

static card* add_card(const cardi* type) {
	auto p = bsdata<card>::add();
	p->id = (const char*)type;
	p->player = 0;
	p->tags = 0;
	p->location = type->getdeck();
	return p;
}

static void add_cards(entitya& cards, const decki* type) {
	for(auto& e : bsdata<cardi>()) {
		if(!e.usedeck())
			continue;
		if(e.location != type)
			continue;
		for(auto i = 0; i < e.count; i++)
			cards.add(add_card(&e));
	}
}

void decki::create() {
	cards.clear();
	add_cards(cards, this);
	cards.shuffle();
}