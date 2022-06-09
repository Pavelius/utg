#include "main.h"

void gamestring::addidentifier(const char* identifier) {
	auto pi = bsdata<indicatori>::find(identifier);
	if(pi) {
		add("%1i", game.active->get((indicator_s)getbsi(pi)));
		return;
	}
	if(equal(identifier, "indicator"))
		add(getnm(bsdata<indicatori>::elements[game.indicator].id));
	else
		stringbuilder::addidentifier(identifier);
}