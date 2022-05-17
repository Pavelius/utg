#include "main.h"

playeri* playeri::last;

void playerdeck::addcards(const char* id, int level) {
	for(auto& e : bsdata<playercardi>()) {
		if(!e.owner)
			continue;
		if(e.owner->id == id && e.level == level)
			deck::add(&e - bsdata<playercardi>::elements);
	}
}