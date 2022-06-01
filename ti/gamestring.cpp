#include "main.h"

void gamestring::addidentifier(const char* identifier) {
	if(equal(identifier, "indicator"))
		add(getnm(bsdata<indicatori>::elements[game.indicator].id));
	else
		stringbuilder::addidentifier(identifier);
}