#include "main.h"

void piratechoose::apply(int index, const void* object) {
	game.addaction(object);
}

bool piratechoose::isallow(int index, const void* object) const {
	auto p = (actioni*)object;
	if(p->is(VisitManyTimes))
		return true;
	if(ismarked(index))
		return false;
	return true;
}