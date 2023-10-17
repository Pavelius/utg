#include "card.h"
#include "deck.h"
#include "script.h"

cardi* last_component;
card* last_card;

void cardi::play() {
	auto push = last_component; last_component = this;
	script_run(effect);
	last_component = push;
}

void card::play() {
	auto p = getcomponent();
	if(!p)
		return;
	auto push = last_card; last_card = this;
	p->play();
	discard();
	last_card = push;
}

void card::discard(bool to_the_top) {
	player = 0;
	location = 0;
	auto deck = getcomponent()->getdeck();
	if(deck) {
		if(to_the_top)
			deck->cards.insert(0, this);
		else
			deck->cards.add(this);
	}
}