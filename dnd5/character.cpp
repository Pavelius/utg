#include "pushvalue.h"
#include "main.h"

void character::clear() {
	memset(this, 0, sizeof(*this));
}

void character::generate() {
	pushvalue push_resid(answers::resid, "generate");
	pushvalue push_header(answers::header, getnm("CharacterGenerate"));
	pushvalue push_column(answers::column_count, -1);
	clear();
	advance("CharacterGenerate", 0);
	if(race->parent)
		advance(race->parent->id, 0);
	if(race)
		advance(race->id, 0);
}