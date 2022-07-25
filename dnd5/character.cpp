#include "main.h"

void character::clear() {
	memset(this, 0, sizeof(*this));
}

void character::generate() {
	auto push_resid = answers::resid;
	auto push_header = answers::header;
	answers::resid = "generate";
	answers::header = getnm("CharacterGenerate");
	clear();
	advance("CharacterGenerate", 0);
	if(race->parent)
		advance(race->parent->id, 0);
	if(race)
		advance(race->id, 0);
	answers::header = push_header;
	answers::resid = push_resid;
}