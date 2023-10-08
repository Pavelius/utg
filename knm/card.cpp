#include "card.h"
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
	last_card = push;
}