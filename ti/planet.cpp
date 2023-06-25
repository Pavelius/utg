#include "main.h"

planeti* last_planet;

int planeti::get(indicator_s v) const {
	switch(v) {
	case Resources: return resources;
	case Influence: return influence;
	default: return 0;
	}
}

void planeti::exhaust() {
	if(flags.is(Exhaust))
		return;
	flags.set(Exhaust);
	draw::warning(getnm("ApplyExhaust"), getnm(id));
}