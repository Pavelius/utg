#include "main.h"

void deck::create(cardtype_s type) {
	for(auto& e : bsdata<cardprotoi>()) {
		if(e.type != type)
			continue;
		auto count = e.count;
		if(!count)
			count = 1;
		auto index = &e - bsdata<cardprotoi>::elements;
		for(auto i = 0; i < count; i++)
			add(index);
	}
}

void deck::shuffle() {
	zshuffle(data, count);
}