#include "card.h"
#include "deck.h"

BSDATA(decki) = {
	{"ActionCards"},
	{"AgendaCards"},
	{"ObjectivesCards"},
	{"SecretObjectivesCards"},
};
BSDATAF(decki)

decki* last_deck;

static card* add_card(const componenti* type) {
	auto p = bsdata<card>::add();
	p->id = (const char*)type;
	p->player = 0;
	p->location = (entity*)type->getdeck();
	return p;
}

static void add_cards(entitya& cards, const decki* type) {
	for(auto& e : bsdata<componenti>()) {
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

void initialize_decks() {
	for(auto& e : bsdata<decki>())
		e.create();
}